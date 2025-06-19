#include "blake256.h"
#include "hash_constants.h"
#include <iomanip>
#include <sstream>
#include <cstring>

// BLAKE256 constants - from the original BLAKE specification
static const uint32_t BLAKE256_IV[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

// Constants for BLAKE256 - these are the first 512 bits of fractional part of pi
static const uint32_t u256[16] = {
    0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
    0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
    0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
    0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917
};

// Permutation tables for BLAKE - exactly as in the reference
const uint8_t sigma[10][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}
};

// Padding bytes - exactly as in reference
static const uint8_t padding[65] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

BLAKE256::BLAKE256() : HashBase(HASH_CONSTANTS::BLAKE256_BLOCK_SIZE) {
    reset();
}

void BLAKE256::reset() {
    resetBase(); // Reset base class state
    
    // Initialize BLAKE256 state exactly like the reference
    memcpy(h, BLAKE256_IV, sizeof(h));
    t[0] = t[1] = 0;
    nullt = 0;
    memset(s, 0, sizeof(s)); // Zero salt
}

void BLAKE256::update(const uint8_t* data, size_t length) {
    if (finalized) {
        throw std::runtime_error("Cannot update after finalization. Call reset() first.");
    }
    
    // BLAKE256-specific update logic following reference implementation
    int left = bufferLength;
    int fill = 64 - left;
    
    // Data left and data received fill a block
    if (left && (length >= fill)) {
        memcpy(buffer + left, data, fill);
        t[0] += 512;
        if (t[0] == 0) t[1]++;
        
        processBlock(buffer);
        data += fill;
        length -= fill;
        left = 0;
        bufferLength = 0;
    }
    
    // Compress blocks of data received
    while (length >= 64) {
        t[0] += 512;
        if (t[0] == 0) t[1]++;
        
        processBlock(data);
        data += 64;
        length -= 64;
    }
    
    // Store any data left
    if (length > 0) {
        memcpy(buffer + left, data, length);
        bufferLength = left + length;
    } else {
        bufferLength = 0;
    }
}

uint32_t BLAKE256::U8TO32_BIG(const uint8_t* p) const {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
           (static_cast<uint32_t>(p[3]));
}

void BLAKE256::U32TO8_BIG(uint8_t* p, uint32_t v) const {
    p[0] = static_cast<uint8_t>(v >> 24);
    p[1] = static_cast<uint8_t>(v >> 16);
    p[2] = static_cast<uint8_t>(v >> 8);
    p[3] = static_cast<uint8_t>(v);
}

uint32_t BLAKE256::rotateRight(uint32_t x, unsigned int n) const {
    return (x >> n) | (x << (32 - n));
}

void BLAKE256::G(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t x, uint32_t y) const {
    // BLAKE256 G function - x and y already have the XOR with constants applied
    a += x + b;
    d = rotateRight(d ^ a, 16);
    c += d;
    b = rotateRight(b ^ c, 12);
    a += y + b;
    d = rotateRight(d ^ a, 8);
    c += d;
    b = rotateRight(b ^ c, 7);
}

void BLAKE256::compress(const uint32_t* block) {
    uint32_t v[16], m[16];
    uint32_t i;
    
    // Convert block to message words 
    for (i = 0; i < 16; ++i) {
        m[i] = block[i];
    }
    
    // Initialize working variables v[0..15] exactly like reference
    for (i = 0; i < 8; ++i) {
        v[i] = h[i];
    }
    
    v[8] = s[0] ^ u256[0];
    v[9] = s[1] ^ u256[1];
    v[10] = s[2] ^ u256[2];
    v[11] = s[3] ^ u256[3];
    v[12] = u256[4];
    v[13] = u256[5];
    v[14] = u256[6];
    v[15] = u256[7];
    
    // Don't xor t when the block is only padding
    if (!nullt) {
        v[12] ^= t[0];
        v[13] ^= t[0];
        v[14] ^= t[1];
        v[15] ^= t[1];
    }
    
    // 14 rounds for BLAKE256 using the exact reference pattern
    for (i = 0; i < 14; ++i) {
        // Column step
        G(v[0], v[4], v[8], v[12], m[sigma[i % 10][0]] ^ u256[sigma[i % 10][1]], m[sigma[i % 10][1]] ^ u256[sigma[i % 10][0]]);
        G(v[1], v[5], v[9], v[13], m[sigma[i % 10][2]] ^ u256[sigma[i % 10][3]], m[sigma[i % 10][3]] ^ u256[sigma[i % 10][2]]);
        G(v[2], v[6], v[10], v[14], m[sigma[i % 10][4]] ^ u256[sigma[i % 10][5]], m[sigma[i % 10][5]] ^ u256[sigma[i % 10][4]]);
        G(v[3], v[7], v[11], v[15], m[sigma[i % 10][6]] ^ u256[sigma[i % 10][7]], m[sigma[i % 10][7]] ^ u256[sigma[i % 10][6]]);
        
        // Diagonal step
        G(v[0], v[5], v[10], v[15], m[sigma[i % 10][8]] ^ u256[sigma[i % 10][9]], m[sigma[i % 10][9]] ^ u256[sigma[i % 10][8]]);
        G(v[1], v[6], v[11], v[12], m[sigma[i % 10][10]] ^ u256[sigma[i % 10][11]], m[sigma[i % 10][11]] ^ u256[sigma[i % 10][10]]);
        G(v[2], v[7], v[8], v[13], m[sigma[i % 10][12]] ^ u256[sigma[i % 10][13]], m[sigma[i % 10][13]] ^ u256[sigma[i % 10][12]]);
        G(v[3], v[4], v[9], v[14], m[sigma[i % 10][14]] ^ u256[sigma[i % 10][15]], m[sigma[i % 10][15]] ^ u256[sigma[i % 10][14]]);
    }
    
    // Finalization - exactly like reference
    for (i = 0; i < 16; ++i) {
        h[i % 8] ^= v[i];
    }
    
    for (i = 0; i < 8; ++i) {
        h[i] ^= s[i % 4];
    }
}

void BLAKE256::processBlock(const uint8_t* block) {
    uint32_t words[16];
    
    // Convert block to 32-bit words (big-endian)
    for (int i = 0; i < 16; ++i) {
        words[i] = U8TO32_BIG(block + i * 4);
    }
    
    // Update counter - this happens in the update method, not here for BLAKE
    compress(words);
}

void BLAKE256::addPadding() {
    uint8_t msglen[8], zo = 0x01, oo = 0x81;
    uint32_t lo = t[0] + (bufferLength << 3), hi = t[1];
    
    // Support for hashing more than 2^32 bits
    if (lo < (bufferLength << 3)) hi++;
    
    U32TO8_BIG(msglen + 0, hi);
    U32TO8_BIG(msglen + 4, lo);
    
    if (bufferLength == 55) { // One padding byte
        t[0] -= 8;
        buffer[bufferLength++] = oo;
        // Will add length to this block
    } else {
        if (bufferLength < 55) { // Enough space to fill the block
            if (!bufferLength) nullt = 1;
            
            t[0] -= 440 - (bufferLength << 3);
            
            // Add padding
            memcpy(buffer + bufferLength, padding, 55 - bufferLength);
            bufferLength = 55;
        } else { // Need 2 compressions
            t[0] -= 512 - (bufferLength << 3);
            
            // Fill current block with padding
            memcpy(buffer + bufferLength, padding, 64 - bufferLength);
            
            // Process current block
            processBlock(buffer);
            
            // Start new block
            bufferLength = 0;
            memset(buffer, 0, blockSize);
            
            t[0] -= 440;
            
            // Add padding for the second block
            memcpy(buffer, padding + 1, 55);
            bufferLength = 55;
            nullt = 1;
        }
        
        buffer[bufferLength++] = zo;
        t[0] -= 8;
    }
    
    t[0] -= 64;
    
    // Add message length
    memcpy(buffer + bufferLength, msglen, 8);
    bufferLength += 8;
    
    // Process final block
    processBlock(buffer);
}

std::string BLAKE256::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call finalize() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(8) << h[i];
    }
    
    return ss.str();
}

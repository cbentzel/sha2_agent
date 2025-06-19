#include "blake512.h"
#include "hash_constants.h"
#include <iomanip>
#include <sstream>
#include <cstring>

// BLAKE512 constants
static const uint64_t BLAKE512_IV[8] = {
    0x6A09E667F3BCC908ULL, 0xBB67AE8584CAA73BULL, 0x3C6EF372FE94F82BULL, 0xA54FF53A5F1D36F1ULL,
    0x510E527FADE682D1ULL, 0x9B05688C2B3E6C1FULL, 0x1F83D9ABFB41BD6BULL, 0x5BE0CD19137E2179ULL
};

static const uint64_t BLAKE512_C[16] = {
    0x243F6A8885A308D3ULL, 0x13198A2E03707344ULL, 0xA4093822299F31D0ULL, 0x082EFA98EC4E6C89ULL,
    0x452821E638D01377ULL, 0xBE5466CF34E90C6CULL, 0xC0AC29B7C97C50DDULL, 0x3F84D5B5B5470917ULL,
    0x9216D5D98979FB1BULL, 0xD1310BA698DFB5ACULL, 0x2FFD72DBD01ADFB7ULL, 0xB8E1AFED6A267E96ULL,
    0xBA7C9045F12C7F99ULL, 0x24A19947B3916CF7ULL, 0x0801F2E2858EFC16ULL, 0x636920D871574E69ULL
};

// Same permutation tables as BLAKE256
static const int SIGMA[10][16] = {
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

BLAKE512::BLAKE512() : HashBase(HASH_CONSTANTS::BLAKE512_BLOCK_SIZE) {
    reset();
}

void BLAKE512::reset() {
    resetBase(); // Reset base class state
    
    // Initialize BLAKE512 state
    memcpy(h, BLAKE512_IV, sizeof(h));
    memset(s, 0, sizeof(s)); // Zero salt
    t[0] = t[1] = 0; // Zero counter
}

uint64_t BLAKE512::rotateRight(uint64_t x, unsigned int n) const {
    return (x >> n) | (x << (64 - n));
}

void BLAKE512::G(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t x, uint64_t y) const {
    a = a + b + (x ^ BLAKE512_C[y & 15]);
    d = rotateRight(d ^ a, 32);
    c = c + d;
    b = rotateRight(b ^ c, 25);
    a = a + b + (y ^ BLAKE512_C[x & 15]);
    d = rotateRight(d ^ a, 16);
    c = c + d;
    b = rotateRight(b ^ c, 11);
}

void BLAKE512::compress(const uint64_t* block, bool lastBlock) {
    uint64_t v[16];
    
    // Initialize working variables
    for (int i = 0; i < 8; ++i) {
        v[i] = h[i];
        v[i + 8] = BLAKE512_C[i];
    }
    
    // Add salt
    for (int i = 0; i < 4; ++i) {
        v[i + 12] ^= s[i];
    }
    
    // Add counter
    if (!lastBlock) {
        v[12] ^= t[0];
        v[13] ^= t[0];
        v[14] ^= t[1];
        v[15] ^= t[1];
    } else {
        v[14] ^= 0xFFFFFFFFFFFFFFFFULL;
        v[15] ^= 0xFFFFFFFFFFFFFFFFULL;
    }
    
    // 16 rounds
    for (int r = 0; r < 16; ++r) {
        const int* s = SIGMA[r % 10];
        
        // Column steps
        G(v[0], v[4], v[8], v[12], block[s[0]], block[s[1]]);
        G(v[1], v[5], v[9], v[13], block[s[2]], block[s[3]]);
        G(v[2], v[6], v[10], v[14], block[s[4]], block[s[5]]);
        G(v[3], v[7], v[11], v[15], block[s[6]], block[s[7]]);
        
        // Diagonal steps
        G(v[0], v[5], v[10], v[15], block[s[8]], block[s[9]]);
        G(v[1], v[6], v[11], v[12], block[s[10]], block[s[11]]);
        G(v[2], v[7], v[8], v[13], block[s[12]], block[s[13]]);
        G(v[3], v[4], v[9], v[14], block[s[14]], block[s[15]]);
    }
    
    // Finalize
    for (int i = 0; i < 8; ++i) {
        h[i] ^= s[i % 4] ^ v[i] ^ v[i + 8];
    }
}

void BLAKE512::processBlock(const uint8_t* block) {
    uint64_t words[16];
    
    // Convert block to 64-bit words (big-endian)
    for (int i = 0; i < 16; ++i) {
        words[i] = (static_cast<uint64_t>(block[i * 8]) << 56) |
                   (static_cast<uint64_t>(block[i * 8 + 1]) << 48) |
                   (static_cast<uint64_t>(block[i * 8 + 2]) << 40) |
                   (static_cast<uint64_t>(block[i * 8 + 3]) << 32) |
                   (static_cast<uint64_t>(block[i * 8 + 4]) << 24) |
                   (static_cast<uint64_t>(block[i * 8 + 5]) << 16) |
                   (static_cast<uint64_t>(block[i * 8 + 6]) << 8) |
                   (static_cast<uint64_t>(block[i * 8 + 7]));
    }
    
    // Increment bit counter (128-bit)
    t[0] += 1024;
    if (t[0] < 1024) {
        t[1]++; // Carry
    }
    
    compress(words, false);
}

void BLAKE512::addPadding() {
    uint64_t totalBits = totalLength * 8;
    
    // Check buffer bounds before writing padding bit
    if (bufferLength >= blockSize) {
        throw std::runtime_error("Buffer overflow detected in addPadding");
    }
    
    // Add padding bit
    buffer[bufferLength++] = 0x80;
    
    // Calculate remaining space
    size_t remaining = blockSize - bufferLength;
    
    // If we don't have enough space for the length (16 bytes), pad this block and start a new one
    if (remaining < 17) {
        // Fill the rest with zeros
        memset(buffer + bufferLength, 0, remaining);
        
        // Process this block
        uint64_t words[16];
        for (int i = 0; i < 16; ++i) {
            words[i] = (static_cast<uint64_t>(buffer[i * 8]) << 56) |
                       (static_cast<uint64_t>(buffer[i * 8 + 1]) << 48) |
                       (static_cast<uint64_t>(buffer[i * 8 + 2]) << 40) |
                       (static_cast<uint64_t>(buffer[i * 8 + 3]) << 32) |
                       (static_cast<uint64_t>(buffer[i * 8 + 4]) << 24) |
                       (static_cast<uint64_t>(buffer[i * 8 + 5]) << 16) |
                       (static_cast<uint64_t>(buffer[i * 8 + 6]) << 8) |
                       (static_cast<uint64_t>(buffer[i * 8 + 7]));
        }
        
        if (bufferLength > 111) {
            t[0] += (bufferLength - 111) * 8;
            if (t[0] < (bufferLength - 111) * 8) {
                t[1]++; // Carry
            }
        }
        compress(words, false);
        
        // Start new block
        bufferLength = 0;
        remaining = blockSize;
    }
    
    // Fill with zeros until we have 16 bytes left
    size_t zeroBytes = remaining - 17;
    memset(buffer + bufferLength, 0, zeroBytes);
    bufferLength += zeroBytes;
    
    // Add the null byte (0x01 for BLAKE512)
    buffer[bufferLength++] = 0x01;
    
    // Add length in bits as 128-bit big-endian integer (we use only lower 64 bits)
    // Upper 64 bits (always 0 for practical input sizes)
    for (int i = 7; i >= 0; --i) {
        buffer[bufferLength++] = 0x00;
    }
    // Lower 64 bits
    for (int i = 7; i >= 0; --i) {
        buffer[bufferLength++] = static_cast<uint8_t>(totalBits >> (i * 8));
    }
    
    // Process the final block
    uint64_t words[16];
    for (int i = 0; i < 16; ++i) {
        words[i] = (static_cast<uint64_t>(buffer[i * 8]) << 56) |
                   (static_cast<uint64_t>(buffer[i * 8 + 1]) << 48) |
                   (static_cast<uint64_t>(buffer[i * 8 + 2]) << 40) |
                   (static_cast<uint64_t>(buffer[i * 8 + 3]) << 32) |
                   (static_cast<uint64_t>(buffer[i * 8 + 4]) << 24) |
                   (static_cast<uint64_t>(buffer[i * 8 + 5]) << 16) |
                   (static_cast<uint64_t>(buffer[i * 8 + 6]) << 8) |
                   (static_cast<uint64_t>(buffer[i * 8 + 7]));
    }
    
    t[0] += (totalLength % 128) * 8;
    if (t[0] < (totalLength % 128) * 8) {
        t[1]++; // Carry
    }
    compress(words, true);
}

std::string BLAKE512::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call finalize() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(16) << h[i];
    }
    
    return ss.str();
}

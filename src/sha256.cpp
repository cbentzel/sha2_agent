#include "sha256.h"
#include <iomanip>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <limits>

// SHA256 constants as defined in FIPS 180-4
const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256() {
    reset();
}

void SHA256::reset() {
    // Initial hash values (FIPS 180-4)
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
    
    bufferLength = 0;
    totalLength = 0;
    finalized = false;
    std::memset(buffer, 0, sizeof(buffer));
}

void SHA256::update(const uint8_t* data, size_t length) {
    if (finalized) {
        throw std::runtime_error("Cannot update after finalization. Call reset() first.");
    }
    
    for (size_t i = 0; i < length; ++i) {
        buffer[bufferLength++] = data[i];
        totalLength++;
        
        if (bufferLength == 64) {
            processBlock(buffer);
            bufferLength = 0;
        }
    }
}

void SHA256::processBlock(const uint8_t* block) {
    uint32_t w[64];
    
    // Initialize first 16 words from the block
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<uint32_t>(block[i * 4]) << 24) |
               (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
               (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
               (static_cast<uint32_t>(block[i * 4 + 3]));
    }
    
    // Extend the first 16 words into the remaining 48 words
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = sigma0(w[i - 15]);
        uint32_t s1 = sigma1(w[i - 2]);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    
    // Initialize working variables
    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    uint32_t f = state[5];
    uint32_t g = state[6];
    uint32_t h = state[7];
    
    // Main loop
    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = bigSigma1(e);
        uint32_t ch = choose(e, f, g);
        uint32_t temp1 = h + S1 + ch + K[i] + w[i];
        uint32_t S0 = bigSigma0(a);
        uint32_t maj = majority(a, b, c);
        uint32_t temp2 = S0 + maj;
        
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    
    // Add the compressed chunk to the current hash value
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
    
    // Clear sensitive data from stack
    std::memset(w, 0, sizeof(w));
    a = b = c = d = e = f = g = h = 0;
}

void SHA256::finalize() {
    if (finalized) {
        return; // Already finalized, avoid double processing
    }
    addPadding();
    finalized = true;
}

void SHA256::addPadding() {
    // Check for potential overflow when converting to bits
    if (totalLength > UINT64_MAX / 8) {
        // This should never happen in practice, but handle gracefully
        throw std::overflow_error("Input too large for SHA256 processing");
    }
    uint64_t totalBits = totalLength * 8;
    
    // Check buffer bounds before writing padding bit
    if (bufferLength >= 64) {
        throw std::runtime_error("Buffer overflow detected in addPadding");
    }
    
    // Add padding bit
    buffer[bufferLength++] = 0x80;
    
    // Add zeros until we have 56 bytes in the buffer (8 bytes left for length)
    if (bufferLength > 56) {
        // Fill the rest of this block with zeros and process it
        while (bufferLength < 64) {
            buffer[bufferLength++] = 0x00;
        }
        processBlock(buffer);
        bufferLength = 0;
    }
    
    // Fill with zeros until we have 56 bytes
    while (bufferLength < 56) {
        buffer[bufferLength++] = 0x00;
    }
    
    // Append length in bits as 64-bit big-endian integer
    for (int i = 7; i >= 0; --i) {
        if (bufferLength >= 64) {
            throw std::runtime_error("Buffer overflow detected while appending length");
        }
        buffer[bufferLength++] = static_cast<uint8_t>(totalBits >> (i * 8));
    }
    
    // Process the final block
    processBlock(buffer);
}

std::string SHA256::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call processStream() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(8) << state[i];
    }
    
    return ss.str();
}

uint32_t SHA256::rightRotate(uint32_t value, unsigned int count) const {
    // Ensure count is within valid range to prevent undefined behavior
    count &= 31; // Equivalent to count % 32, but faster
    return (value >> count) | (value << (32 - count));
}

uint32_t SHA256::choose(uint32_t x, uint32_t y, uint32_t z) const {
    return (x & y) ^ (~x & z);
}

uint32_t SHA256::majority(uint32_t x, uint32_t y, uint32_t z) const {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t SHA256::sigma0(uint32_t x) const {
    return rightRotate(x, 7) ^ rightRotate(x, 18) ^ (x >> 3);
}

uint32_t SHA256::sigma1(uint32_t x) const {
    return rightRotate(x, 17) ^ rightRotate(x, 19) ^ (x >> 10);
}

uint32_t SHA256::bigSigma0(uint32_t x) const {
    return rightRotate(x, 2) ^ rightRotate(x, 13) ^ rightRotate(x, 22);
}

uint32_t SHA256::bigSigma1(uint32_t x) const {
    return rightRotate(x, 6) ^ rightRotate(x, 11) ^ rightRotate(x, 25);
}

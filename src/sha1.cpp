#include "sha1.h"
#include "hash_constants.h"
#include <iomanip>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <limits>

SHA1::SHA1() : HashBase(HASH_CONSTANTS::SHA1_BLOCK_SIZE) {
    reset();
}

void SHA1::reset() {
    resetBase(); // Reset base class state
    
    // SHA1 initial hash values
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
    state[4] = 0xc3d2e1f0;
}

void SHA1::processBlock(const uint8_t* block) {
    uint32_t w[80];
    
    // Initialize first 16 words from the block (big-endian)
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<uint32_t>(block[i * 4]) << 24) |
               (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
               (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
               (static_cast<uint32_t>(block[i * 4 + 3]));
    }
    
    // Extend the first 16 words into the remaining 64 words
    for (int i = 16; i < 80; ++i) {
        w[i] = leftRotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }
    
    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];
    
    // Main loop
    for (int i = 0; i < 80; ++i) {
        uint32_t f, k;
        
        if (i < 20) {
            f = (b & c) | (~b & d);
            k = 0x5a827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ed9eba1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8f1bbcdc;
        } else {
            f = b ^ c ^ d;
            k = 0xca62c1d6;
        }
        
        uint32_t temp = leftRotate(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = leftRotate(b, 30);
        b = a;
        a = temp;
    }
    
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void SHA1::addPadding() {
    // Check for overflow
    if (totalLength > UINT64_MAX / 8) {
        throw std::overflow_error("Input too large for SHA1 processing");
    }
    
    uint64_t totalBits = totalLength * 8;
    
    // Check buffer bounds
    if (bufferLength >= blockSize) {
        throw std::runtime_error("Buffer overflow detected in addPadding");
    }
    
    // Add padding bit
    buffer[bufferLength++] = 0x80;
    
    // Add zeros until we have 56 bytes (8 bytes left for length)
    if (bufferLength > 56) {
        while (bufferLength < blockSize) {
            buffer[bufferLength++] = 0x00;
        }
        processBlock(buffer);
        bufferLength = 0;
    }
    
    while (bufferLength < 56) {
        buffer[bufferLength++] = 0x00;
    }
    
    // Append length in bits as 64-bit big-endian integer
    for (int i = 7; i >= 0; --i) {
        if (bufferLength >= blockSize) {
            throw std::runtime_error("Buffer overflow detected while appending length");
        }
        buffer[bufferLength++] = static_cast<uint8_t>(totalBits >> (i * 8));
    }
    
    processBlock(buffer);
}

std::string SHA1::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call finalize() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (int i = 0; i < 5; ++i) {
        ss << std::setw(8) << state[i];
    }
    
    return ss.str();
}

uint32_t SHA1::leftRotate(uint32_t value, unsigned int count) const {
    count &= 31; // Prevent undefined behavior
    return (value << count) | (value >> (32 - count));
}

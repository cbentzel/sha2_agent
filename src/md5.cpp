#include "md5.h"
#include <iomanip>
#include <sstream>
#include <cstring>
#include <stdexcept>

// MD5 round constants
static const uint32_t T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

MD5::MD5() {
    reset();
}

void MD5::reset() {
    // MD5 initial hash values
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
    
    bufferLength = 0;
    totalLength = 0;
    finalized = false;
    std::memset(buffer, 0, sizeof(buffer));
}

void MD5::update(const uint8_t* data, size_t length) {
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

void MD5::finalize() {
    if (finalized) {
        return; // Already finalized
    }
    addPadding();
    finalized = true;
}

void MD5::processBlock(const uint8_t* block) {
    uint32_t w[16];
    
    // Convert block to 32-bit words (little-endian)
    for (int i = 0; i < 16; ++i) {
        w[i] = static_cast<uint32_t>(block[i * 4]) |
               (static_cast<uint32_t>(block[i * 4 + 1]) << 8) |
               (static_cast<uint32_t>(block[i * 4 + 2]) << 16) |
               (static_cast<uint32_t>(block[i * 4 + 3]) << 24);
    }
    
    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    
    // MD5 rounds
    for (int i = 0; i < 64; ++i) {
        uint32_t f, g;
        
        if (i < 16) {
            f = F(b, c, d);
            g = i;
        } else if (i < 32) {
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        } else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }
        
        uint32_t temp = d;
        d = c;
        c = b;
        
        uint32_t sum = a + f + T[i] + w[g];
        
        // Rotation amounts for each round
        static const unsigned int rotations[64] = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
        };
        
        b = b + leftRotate(sum, rotations[i]);
        a = temp;
    }
    
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void MD5::addPadding() {
    // Check for overflow
    if (totalLength > UINT64_MAX / 8) {
        throw std::overflow_error("Input too large for MD5 processing");
    }
    
    uint64_t totalBits = totalLength * 8;
    
    // Check buffer bounds
    if (bufferLength >= 64) {
        throw std::runtime_error("Buffer overflow detected in addPadding");
    }
    
    // Add padding bit
    buffer[bufferLength++] = 0x80;
    
    // Add zeros until we have 56 bytes (8 bytes left for length)
    if (bufferLength > 56) {
        while (bufferLength < 64) {
            buffer[bufferLength++] = 0x00;
        }
        processBlock(buffer);
        bufferLength = 0;
    }
    
    while (bufferLength < 56) {
        buffer[bufferLength++] = 0x00;
    }
    
    // Append length in bits as 64-bit little-endian integer
    for (int i = 0; i < 8; ++i) {
        if (bufferLength >= 64) {
            throw std::runtime_error("Buffer overflow detected while appending length");
        }
        buffer[bufferLength++] = static_cast<uint8_t>(totalBits >> (i * 8));
    }
    
    processBlock(buffer);
}

std::string MD5::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call finalize() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // MD5 output is little-endian
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ss << std::setw(2) << static_cast<unsigned int>((state[i] >> (j * 8)) & 0xff);
        }
    }
    
    return ss.str();
}

uint32_t MD5::leftRotate(uint32_t value, unsigned int count) const {
    count &= 31; // Prevent undefined behavior
    return (value << count) | (value >> (32 - count));
}

uint32_t MD5::F(uint32_t x, uint32_t y, uint32_t z) const {
    return (x & y) | (~x & z);
}

uint32_t MD5::G(uint32_t x, uint32_t y, uint32_t z) const {
    return (x & z) | (y & ~z);
}

uint32_t MD5::H(uint32_t x, uint32_t y, uint32_t z) const {
    return x ^ y ^ z;
}

uint32_t MD5::I(uint32_t x, uint32_t y, uint32_t z) const {
    return y ^ (x | ~z);
}

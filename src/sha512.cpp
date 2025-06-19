#include "sha512.h"
#include "hash_constants.h"
#include <iomanip>
#include <sstream>
#include <cstring>

// SHA512 constants (first 64 bits of the fractional parts of the cube roots of the first 80 primes)
const uint64_t K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

SHA512::SHA512() : HashBase(HASH_CONSTANTS::SHA512_BLOCK_SIZE) {
    reset();
}

void SHA512::reset() {
    resetBase(); // Reset base class state
    
    // Initialize SHA512-specific state values (FIPS 180-4)
    state[0] = 0x6a09e667f3bcc908ULL;
    state[1] = 0xbb67ae8584caa73bULL;
    state[2] = 0x3c6ef372fe94f82bULL;
    state[3] = 0xa54ff53a5f1d36f1ULL;
    state[4] = 0x510e527fade682d1ULL;
    state[5] = 0x9b05688c2b3e6c1fULL;
    state[6] = 0x1f83d9abfb41bd6bULL;
    state[7] = 0x5be0cd19137e2179ULL;
}

void SHA512::processBlock(const uint8_t* block) {
    uint64_t w[80];
    
    // Initialize first 16 words from the block (big-endian)
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<uint64_t>(block[i * 8]) << 56) |
               (static_cast<uint64_t>(block[i * 8 + 1]) << 48) |
               (static_cast<uint64_t>(block[i * 8 + 2]) << 40) |
               (static_cast<uint64_t>(block[i * 8 + 3]) << 32) |
               (static_cast<uint64_t>(block[i * 8 + 4]) << 24) |
               (static_cast<uint64_t>(block[i * 8 + 5]) << 16) |
               (static_cast<uint64_t>(block[i * 8 + 6]) << 8) |
               (static_cast<uint64_t>(block[i * 8 + 7]));
    }
    
    // Extend the first 16 words into the remaining 64 words
    for (int i = 16; i < 80; ++i) {
        uint64_t s0 = sigma0(w[i - 15]);
        uint64_t s1 = sigma1(w[i - 2]);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    
    // Initialize working variables
    uint64_t a = state[0];
    uint64_t b = state[1];
    uint64_t c = state[2];
    uint64_t d = state[3];
    uint64_t e = state[4];
    uint64_t f = state[5];
    uint64_t g = state[6];
    uint64_t h = state[7];
    
    // Main loop
    for (int i = 0; i < 80; ++i) {
        uint64_t S1 = bigSigma1(e);
        uint64_t ch = choose(e, f, g);
        uint64_t temp1 = h + S1 + ch + K[i] + w[i];
        uint64_t S0 = bigSigma0(a);
        uint64_t maj = majority(a, b, c);
        uint64_t temp2 = S0 + maj;
        
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

void SHA512::addPadding() {
    // Check for potential overflow when converting to bits  
    if (totalLength > UINT64_MAX / 8) {
        throw std::overflow_error("Input too large for SHA512 processing");
    }
    uint64_t totalBits = totalLength * 8;
    
    // Check buffer bounds before writing padding bit
    if (bufferLength >= blockSize) {
        throw std::runtime_error("Buffer overflow detected in addPadding");
    }
    
    // Add padding bit
    buffer[bufferLength++] = 0x80;
    
    // Add zeros until we have 112 bytes in the buffer (16 bytes left for length)
    if (bufferLength > 112) {
        // Fill the rest of this block with zeros and process it
        while (bufferLength < blockSize) {
            buffer[bufferLength++] = 0x00;
        }
        processBlock(buffer);
        bufferLength = 0;
    }
    
    // Fill with zeros until we have 112 bytes
    while (bufferLength < 112) {
        buffer[bufferLength++] = 0x00;
    }
    
    // Append length in bits as 128-bit big-endian integer (we only use lower 64 bits)
    // Upper 64 bits (always 0 for practical input sizes)
    for (int i = 7; i >= 0; --i) {
        if (bufferLength >= blockSize) {
            throw std::runtime_error("Buffer overflow detected while appending length");
        }
        buffer[bufferLength++] = 0x00;
    }
    
    // Lower 64 bits 
    for (int i = 7; i >= 0; --i) {
        if (bufferLength >= blockSize) {
            throw std::runtime_error("Buffer overflow detected while appending length");
        }
        buffer[bufferLength++] = static_cast<uint8_t>(totalBits >> (i * 8));
    }
    
    // Process the final block
    processBlock(buffer);
}

std::string SHA512::getHash() const {
    if (!finalized) {
        throw std::runtime_error("Cannot get hash before finalization. Call processStream() first.");
    }
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(16) << state[i];
    }
    
    return ss.str();
}

uint64_t SHA512::rightRotate(uint64_t value, unsigned int count) const {
    // Ensure count is within valid range to prevent undefined behavior
    count &= 63; // Equivalent to count % 64, but faster
    return (value >> count) | (value << (64 - count));
}

uint64_t SHA512::choose(uint64_t x, uint64_t y, uint64_t z) const {
    return (x & y) ^ (~x & z);
}

uint64_t SHA512::majority(uint64_t x, uint64_t y, uint64_t z) const {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint64_t SHA512::sigma0(uint64_t x) const {
    return rightRotate(x, 1) ^ rightRotate(x, 8) ^ (x >> 7);
}

uint64_t SHA512::sigma1(uint64_t x) const {
    return rightRotate(x, 19) ^ rightRotate(x, 61) ^ (x >> 6);
}

uint64_t SHA512::bigSigma0(uint64_t x) const {
    return rightRotate(x, 28) ^ rightRotate(x, 34) ^ rightRotate(x, 39);
}

uint64_t SHA512::bigSigma1(uint64_t x) const {
    return rightRotate(x, 14) ^ rightRotate(x, 18) ^ rightRotate(x, 41);
}

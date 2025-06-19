#ifndef HASH_CONSTANTS_H
#define HASH_CONSTANTS_H

#include <cstddef>

namespace HASH_CONSTANTS {
    // Common block and buffer sizes
    static constexpr size_t BLOCK_SIZE_512 = 64;   // 512 bits = 64 bytes (MD5, SHA-1, SHA-256)
    static constexpr size_t BLOCK_SIZE_1024 = 128; // 1024 bits = 128 bytes (SHA-512)
    static constexpr size_t BUFFER_SIZE = 32768;   // 32KB stream processing buffer
    
    // Hash output sizes
    static constexpr size_t MD5_HASH_SIZE = 16;     // 128 bits
    static constexpr size_t SHA1_HASH_SIZE = 20;    // 160 bits
    static constexpr size_t SHA256_HASH_SIZE = 32;  // 256 bits
    static constexpr size_t SHA512_HASH_SIZE = 64;  // 512 bits
    static constexpr size_t BLAKE256_HASH_SIZE = 32; // 256 bits
    static constexpr size_t BLAKE512_HASH_SIZE = 64; // 512 bits
    
    // Algorithm-specific block sizes  
    static constexpr size_t MD5_BLOCK_SIZE = BLOCK_SIZE_512;
    static constexpr size_t SHA1_BLOCK_SIZE = BLOCK_SIZE_512;
    static constexpr size_t SHA256_BLOCK_SIZE = BLOCK_SIZE_512;
    static constexpr size_t SHA512_BLOCK_SIZE = BLOCK_SIZE_1024;
    static constexpr size_t BLAKE256_BLOCK_SIZE = BLOCK_SIZE_512;
    static constexpr size_t BLAKE512_BLOCK_SIZE = BLOCK_SIZE_1024;
    
    // Padding constants
    static constexpr uint8_t PADDING_BIT = 0x80;
    static constexpr size_t LENGTH_FIELD_SIZE = 8;   // 64-bit length field
    static constexpr size_t PADDING_BOUNDARY = 56;   // Leave 8 bytes for length
}

#endif // HASH_CONSTANTS_H

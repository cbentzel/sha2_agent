#ifndef BLAKE256_H
#define BLAKE256_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class BLAKE256 : public HashBase {
public:
    BLAKE256();
    
    // HashBase implementation
    void reset() override;
    void update(const uint8_t* data, size_t length) override;  // Override to handle BLAKE counter
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::BLAKE256_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::BLAKE256_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "BLAKE256"; }

private:
    // BLAKE256 state (8 32-bit words + 4 salt words + 2 counter words)
    uint32_t h[8];      // Hash state
    uint32_t s[4];      // Salt (we use zero salt)
    uint32_t t[2];      // Counter (64-bit total, split into two 32-bit words)
    int nullt;          // Flag for padding-only blocks
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    
    // BLAKE-specific functions
    void G(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t x, uint32_t y) const;
    uint32_t rotateRight(uint32_t x, unsigned int n) const;
    void compress(const uint32_t* block);
    uint32_t U8TO32_BIG(const uint8_t* p) const;
    void U32TO8_BIG(uint8_t* p, uint32_t v) const;
};

#endif // BLAKE256_H

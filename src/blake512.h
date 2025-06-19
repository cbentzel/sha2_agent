#ifndef BLAKE512_H
#define BLAKE512_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class BLAKE512 : public HashBase {
public:
    BLAKE512();
    
    // HashBase implementation
    void reset() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::BLAKE512_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::BLAKE512_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "BLAKE512"; }

private:
    // BLAKE512 state (8 64-bit words + 4 salt words + 2 counter words)
    uint64_t h[8];      // Hash state
    uint64_t s[4];      // Salt (we use zero salt)
    uint64_t t[2];      // Counter (128-bit)
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    
    // BLAKE-specific functions
    void G(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t x, uint64_t y) const;
    uint64_t rotateRight(uint64_t x, unsigned int n) const;
    void compress(const uint64_t* block, bool lastBlock = false);
};

#endif // BLAKE512_H

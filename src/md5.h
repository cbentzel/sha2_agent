#ifndef MD5_H
#define MD5_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class MD5 : public HashBase {
public:
    MD5();
    
    // HashBase implementation
    void reset() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::MD5_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::MD5_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "MD5"; }

private:
    // MD5 state
    uint32_t state[4];
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    uint32_t leftRotate(uint32_t value, unsigned int count) const;
    uint32_t F(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t G(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t H(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t I(uint32_t x, uint32_t y, uint32_t z) const;
};

#endif // MD5_H

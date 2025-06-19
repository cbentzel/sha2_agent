#ifndef SHA256_H
#define SHA256_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class SHA256 : public HashBase {
public:
    SHA256();
    
    // HashBase implementation
    void reset() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::SHA256_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::SHA256_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "SHA256"; }

private:
    // Internal state
    uint32_t state[8];
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    uint32_t rightRotate(uint32_t value, unsigned int count) const;
    uint32_t choose(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t majority(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t sigma0(uint32_t x) const;
    uint32_t sigma1(uint32_t x) const;
    uint32_t bigSigma0(uint32_t x) const;
    uint32_t bigSigma1(uint32_t x) const;
};

#endif // SHA256_H

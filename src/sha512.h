#ifndef SHA512_H
#define SHA512_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class SHA512 : public HashBase {
public:
    SHA512();
    
    // HashBase implementation
    void reset() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::SHA512_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::SHA512_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "SHA512"; }

private:
    // Internal state (8 64-bit words)
    uint64_t state[8];
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    uint64_t rightRotate(uint64_t value, unsigned int count) const;
    uint64_t choose(uint64_t x, uint64_t y, uint64_t z) const;
    uint64_t majority(uint64_t x, uint64_t y, uint64_t z) const;
    uint64_t sigma0(uint64_t x) const;
    uint64_t sigma1(uint64_t x) const;
    uint64_t bigSigma0(uint64_t x) const;
    uint64_t bigSigma1(uint64_t x) const;
};

#endif // SHA512_H

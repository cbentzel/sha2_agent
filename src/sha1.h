#ifndef SHA1_H
#define SHA1_H

#include "hash_base.h"
#include "hash_constants.h"
#include <cstdint>
#include <string>

class SHA1 : public HashBase {
public:
    SHA1();
    
    // HashBase implementation
    void reset() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return HASH_CONSTANTS::SHA1_BLOCK_SIZE; }
    size_t getHashSize() const override { return HASH_CONSTANTS::SHA1_HASH_SIZE; }
    std::string getAlgorithmName() const override { return "SHA1"; }

private:
    // SHA1 state
    uint32_t state[5];
    
    // Internal methods
    void processBlock(const uint8_t* block) override;
    void addPadding() override;
    uint32_t leftRotate(uint32_t value, unsigned int count) const;
};

#endif // SHA1_H

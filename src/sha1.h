#ifndef SHA1_H
#define SHA1_H

#include "hash_interface.h"
#include <cstdint>
#include <string>

class SHA1 : public HashInterface {
public:
    SHA1();
    
    // HashInterface implementation
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void finalize() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return 64; }
    size_t getHashSize() const override { return 20; }
    std::string getAlgorithmName() const override { return "SHA1"; }
    bool isFinalized() const override { return finalized; }

private:
    // SHA1 state
    uint32_t state[5];
    uint8_t buffer[64];
    size_t bufferLength;
    uint64_t totalLength;
    bool finalized;
    
    // Internal methods
    void processBlock(const uint8_t* block);
    void addPadding();
    uint32_t leftRotate(uint32_t value, unsigned int count) const;
};

#endif // SHA1_H

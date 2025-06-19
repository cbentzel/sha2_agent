#ifndef MD5_H
#define MD5_H

#include "hash_interface.h"
#include <cstdint>
#include <string>

class MD5 : public HashInterface {
public:
    MD5();
    
    // HashInterface implementation
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void finalize() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return 64; }
    size_t getHashSize() const override { return 16; }
    std::string getAlgorithmName() const override { return "MD5"; }
    bool isFinalized() const override { return finalized; }

private:
    // MD5 state
    uint32_t state[4];
    uint8_t buffer[64];
    size_t bufferLength;
    uint64_t totalLength;
    bool finalized;
    
    // Internal methods
    void processBlock(const uint8_t* block);
    void addPadding();
    uint32_t leftRotate(uint32_t value, unsigned int count) const;
    uint32_t F(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t G(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t H(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t I(uint32_t x, uint32_t y, uint32_t z) const;
};

#endif // MD5_H

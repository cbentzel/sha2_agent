#ifndef SHA256_H
#define SHA256_H

#include "hash_interface.h"
#include <cstdint>
#include <string>

class SHA256 : public HashInterface {
public:
    SHA256();
    
    // HashInterface implementation
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void finalize() override;
    std::string getHash() const override;
    size_t getBlockSize() const override { return 64; }
    size_t getHashSize() const override { return 32; }
    std::string getAlgorithmName() const override { return "SHA256"; }
    bool isFinalized() const override { return finalized; }

private:
    // SHA256 constants
    static const uint32_t K[64];
    
    // Internal state
    uint32_t state[8];
    uint8_t buffer[64];
    size_t bufferLength;
    uint64_t totalLength;
    bool finalized;
    
    // Internal methods
    void processBlock(const uint8_t* block);
    void addPadding();
    uint32_t rightRotate(uint32_t value, unsigned int count) const;
    uint32_t choose(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t majority(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t sigma0(uint32_t x) const;
    uint32_t sigma1(uint32_t x) const;
    uint32_t bigSigma0(uint32_t x) const;
    uint32_t bigSigma1(uint32_t x) const;
};

#endif // SHA256_H

#ifndef SHA256_H
#define SHA256_H

#include <cstdint>
#include <string>
#include <iostream>

class SHA256 {
public:
    SHA256();
    
    // Process data from input stream
    void processStream(std::istream& input);
    
    // Get the final hash as a hexadecimal string
    std::string getHash() const;
    
    // Reset the hasher for reuse
    void reset();

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
    void finalize();
    uint32_t rightRotate(uint32_t value, unsigned int count) const;
    uint32_t choose(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t majority(uint32_t x, uint32_t y, uint32_t z) const;
    uint32_t sigma0(uint32_t x) const;
    uint32_t sigma1(uint32_t x) const;
    uint32_t bigSigma0(uint32_t x) const;
    uint32_t bigSigma1(uint32_t x) const;
    void addPadding();
};

#endif // SHA256_H

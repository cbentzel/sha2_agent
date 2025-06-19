#ifndef HASH_BASE_H
#define HASH_BASE_H

#include "hash_interface.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <limits>

/**
 * Base class for hash implementations providing common functionality
 * Reduces code duplication and ensures consistent error handling
 */
class HashBase : public HashInterface {
protected:
    uint8_t buffer[128];         // Buffer large enough for SHA512 (128 bytes), smaller algorithms use less
    size_t bufferLength;
    uint64_t totalLength;
    bool finalized;
    size_t blockSize;            // Algorithm-specific block size
    
    /**
     * Constructor - initializes common state
     */
    HashBase(size_t block_size) : bufferLength(0), totalLength(0), finalized(false), blockSize(block_size) {
        std::memset(buffer, 0, sizeof(buffer));
    }
    
    /**
     * Common update implementation with bounds checking
     * Subclasses should call this from their update() method
     */
    void updateBase(const uint8_t* data, size_t length) {
        if (finalized) {
            throw std::runtime_error("Cannot update after finalization. Call reset() first.");
        }
        
        // Check for potential totalLength overflow before processing
        if (length > UINT64_MAX - totalLength) {
            throw std::overflow_error("Input too large - would cause totalLength overflow");
        }
        
        for (size_t i = 0; i < length; ++i) {
            // Bounds check for buffer
            if (bufferLength >= blockSize) {
                throw std::runtime_error("Internal buffer overflow detected");
            }
            
            buffer[bufferLength++] = data[i];
            totalLength++;
            
            if (bufferLength == blockSize) {
                processBlock(buffer);
                bufferLength = 0;
            }
        }
    }
    
    /**
     * Common reset implementation
     * Subclasses should call this from their reset() method
     */
    void resetBase() {
        bufferLength = 0;
        totalLength = 0;
        finalized = false;
        std::memset(buffer, 0, sizeof(buffer));
    }
    
    /**
     * Common finalize implementation
     * Subclasses should call this from their finalize() method
     */
    void finalizeBase() {
        if (finalized) {
            return; // Already finalized, avoid double processing
        }
        addPadding();
        finalized = true;
    }
    
    /**
     * Pure virtual method for block processing
     * Each algorithm implements its own block processing logic
     */
    virtual void processBlock(const uint8_t* block) = 0;
    
    /**
     * Pure virtual method for padding
     * Each algorithm implements its own padding logic
     */
    virtual void addPadding() = 0;

public:
    // HashInterface implementation - delegate to subclass-specific reset
    virtual void reset() override = 0;
    
    // Common update implementation
    void update(const uint8_t* data, size_t length) override {
        updateBase(data, length);
    }
    
    // Common finalize implementation
    void finalize() override {
        finalizeBase();
    }
    
    bool isFinalized() const override { return finalized; }
};

#endif // HASH_BASE_H

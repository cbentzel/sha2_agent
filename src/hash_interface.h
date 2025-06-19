#ifndef HASH_INTERFACE_H
#define HASH_INTERFACE_H

#include <cstdint>
#include <string>
#include <iostream>

/**
 * Abstract base class for hash algorithms
 * Provides a common interface for all hash implementations
 */
class HashInterface {
public:
    virtual ~HashInterface() = default;
    
    /**
     * Reset the hasher to initial state for reuse
     */
    virtual void reset() = 0;
    
    /**
     * Process a block of data
     * @param data Pointer to data buffer
     * @param length Number of bytes to process
     */
    virtual void update(const uint8_t* data, size_t length) = 0;
    
    /**
     * Finalize the hash computation
     * Must be called before getHash()
     */
    virtual void finalize() = 0;
    
    /**
     * Get the final hash as a hexadecimal string
     * @return Hash value as lowercase hex string
     */
    virtual std::string getHash() const = 0;
    
    /**
     * Get the block size for this hash algorithm
     * @return Block size in bytes
     */
    virtual size_t getBlockSize() const = 0;
    
    /**
     * Get the hash size for this hash algorithm
     * @return Hash size in bytes
     */
    virtual size_t getHashSize() const = 0;
    
    /**
     * Get the name of this hash algorithm
     * @return Algorithm name
     */
    virtual std::string getAlgorithmName() const = 0;
    
    /**
     * Check if the hasher has been finalized
     * @return True if finalized, false otherwise
     */
    virtual bool isFinalized() const = 0;
};

#endif // HASH_INTERFACE_H

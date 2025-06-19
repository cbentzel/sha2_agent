#ifndef STREAM_PROCESSOR_H
#define STREAM_PROCESSOR_H

#include "hash_interface.h"
#include <iostream>
#include <memory>

/**
 * Stream processor for handling input data and feeding it to hash algorithms
 * Provides common streaming functionality for all hash implementations
 */
class StreamProcessor {
public:
    /**
     * Constructor
     * @param hasher Unique pointer to hash implementation
     */
    explicit StreamProcessor(std::unique_ptr<HashInterface> hasher);
    
    /**
     * Process data from input stream
     * @param input Input stream to read from
     */
    void processStream(std::istream& input);
    
    /**
     * Get the final hash result
     * @return Hash as hexadecimal string
     */
    std::string getHash() const;
    
    /**
     * Reset the processor and hasher for reuse
     */
    void reset();
    
    /**
     * Get the algorithm name
     * @return Name of the hash algorithm
     */
    std::string getAlgorithmName() const;

private:
    std::unique_ptr<HashInterface> hasher_;
    static const size_t BUFFER_SIZE = 32768; // 32KB buffer
};

#endif // STREAM_PROCESSOR_H

#ifndef HASH_ERRORS_H
#define HASH_ERRORS_H

#include <stdexcept>
#include <string>

namespace HashErrors {
    
    /**
     * Custom exception for hash-specific errors
     */
    class HashException : public std::runtime_error {
    public:
        explicit HashException(const std::string& message) 
            : std::runtime_error("Hash Error: " + message) {}
    };
    
    /**
     * Exception for algorithm state errors
     */
    class StateException : public HashException {
    public:
        explicit StateException(const std::string& message) 
            : HashException("State Error: " + message) {}
    };
    
    /**
     * Exception for input validation errors
     */
    class InputException : public HashException {
    public:
        explicit InputException(const std::string& message) 
            : HashException("Input Error: " + message) {}
    };
    
    /**
     * Exception for overflow conditions
     */
    class OverflowException : public HashException {
    public:
        explicit OverflowException(const std::string& message) 
            : HashException("Overflow Error: " + message) {}
    };
}

#endif // HASH_ERRORS_H

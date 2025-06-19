#ifndef HASH_FACTORY_H
#define HASH_FACTORY_H

#include "hash_interface.h"
#include <memory>
#include <string>
#include <vector>

/**
 * Factory class for creating hash algorithm instances
 */
class HashFactory {
public:
    /**
     * Create a hash algorithm instance by name
     * @param algorithm Algorithm name (case-insensitive)
     * @return Unique pointer to hash implementation
     * @throws std::invalid_argument if algorithm is not supported
     */
    static std::unique_ptr<HashInterface> createHash(const std::string& algorithm);
    
    /**
     * Get list of supported algorithms
     * @return Vector of algorithm names
     */
    static std::vector<std::string> getSupportedAlgorithms();
    
    /**
     * Check if an algorithm is supported
     * @param algorithm Algorithm name (case-insensitive)
     * @return True if supported, false otherwise
     */
    static bool isSupported(const std::string& algorithm);

private:
    static std::string toLowerCase(const std::string& str);
};

#endif // HASH_FACTORY_H

#include "hash_factory.h"
#include "sha1.h"
#include "sha256.h"
#include "md5.h"
#include <stdexcept>
#include <algorithm>

std::unique_ptr<HashInterface> HashFactory::createHash(const std::string& algorithm) {
    std::string algo = toLowerCase(algorithm);
    
    if (algo == "md5") {
        return std::make_unique<MD5>();
    } else if (algo == "sha1" || algo == "sha128") {
        // Note: SHA-128 isn't a standard algorithm, using SHA-1 instead
        return std::make_unique<SHA1>();
    } else if (algo == "sha256") {
        return std::make_unique<SHA256>();
    } else if (algo == "sha512") {
        // TODO: Implement SHA512
        throw std::invalid_argument("SHA512 not yet implemented");
    } else if (algo == "blake256") {
        // TODO: Implement BLAKE256
        throw std::invalid_argument("BLAKE256 not yet implemented");
    } else if (algo == "blake512") {
        // TODO: Implement BLAKE512
        throw std::invalid_argument("BLAKE512 not yet implemented");
    } else {
        throw std::invalid_argument("Unsupported hash algorithm: " + algorithm);
    }
}

std::vector<std::string> HashFactory::getSupportedAlgorithms() {
    return {
        "MD5",
        "SHA1",
        "SHA256"
        // "SHA512",     // TODO: Implement
        // "BLAKE256",   // TODO: Implement  
        // "BLAKE512"    // TODO: Implement
    };
}

bool HashFactory::isSupported(const std::string& algorithm) {
    std::string algo = toLowerCase(algorithm);
    auto supported = getSupportedAlgorithms();
    
    for (const auto& supportedAlgo : supported) {
        if (toLowerCase(supportedAlgo) == algo) {
            return true;
        }
    }
    
    // Special case for SHA128 -> SHA1
    return algo == "sha128";
}

std::string HashFactory::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

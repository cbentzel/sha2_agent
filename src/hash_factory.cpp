#include "hash_factory.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "md5.h"
#include <stdexcept>
#include <algorithm>

std::unique_ptr<HashInterface> HashFactory::createHash(const std::string& algorithm) {
    std::string algo = toLowerCase(algorithm);
    
    if (algo == "md5") {
        return std::make_unique<MD5>();
    } else if (algo == "sha1") {
        return std::make_unique<SHA1>();
    } else if (algo == "sha256") {
        return std::make_unique<SHA256>();
    } else if (algo == "sha512") {
        return std::make_unique<SHA512>();
    } else if (algo == "blake256") {
        // BLAKE256 is not implemented - complex algorithm requiring significant development
        throw std::invalid_argument("BLAKE256 not implemented - consider using SHA256 instead");
    } else if (algo == "blake512") {
        // BLAKE512 is not implemented - complex algorithm requiring significant development  
        throw std::invalid_argument("BLAKE512 not implemented - consider using SHA512 instead");
    } else {
        throw std::invalid_argument("Unsupported hash algorithm: " + algorithm);
    }
}

std::vector<std::string> HashFactory::getSupportedAlgorithms() {
    return {
        "MD5",
        "SHA1",
        "SHA256",
        "SHA512"
        // Note: BLAKE256 and BLAKE512 are not implemented
        // These are complex algorithms that would require substantial development effort
        // For most use cases, SHA256/SHA512 provide equivalent or better security
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
    
    return false;
}

std::string HashFactory::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

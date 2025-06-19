#include "hash_factory.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "blake256.h"
#include "blake512.h"
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
        return std::make_unique<BLAKE256>();
    } else if (algo == "blake512") {
        return std::make_unique<BLAKE512>();
    } else {
        throw std::invalid_argument("Unsupported hash algorithm: " + algorithm);
    }
}

std::vector<std::string> HashFactory::getSupportedAlgorithms() {
    return {
        "MD5",
        "SHA1",
        "SHA256",
        "SHA512",
        "BLAKE256",
        "BLAKE512"
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

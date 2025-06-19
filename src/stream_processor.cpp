#include "stream_processor.h"
#include <cstring>
#include <stdexcept>

StreamProcessor::StreamProcessor(std::unique_ptr<HashInterface> hasher)
    : hasher_(std::move(hasher)) {
    if (!hasher_) {
        throw std::invalid_argument("Hash implementation cannot be null");
    }
}

void StreamProcessor::processStream(std::istream& input) {
    if (!hasher_) {
        throw std::runtime_error("No hash implementation available");
    }
    
    if (hasher_->isFinalized()) {
        throw std::runtime_error("Cannot process stream after finalization. Call reset() first.");
    }
    
    char buffer[BUFFER_SIZE];
    
    // Initialize buffer to prevent information leakage
    std::memset(buffer, 0, BUFFER_SIZE);
    
    while (input.good()) {
        input.read(buffer, BUFFER_SIZE);
        std::streamsize bytesRead = input.gcount();
        
        if (bytesRead > 0) {
            hasher_->update(reinterpret_cast<const uint8_t*>(buffer), 
                           static_cast<size_t>(bytesRead));
        }
        
        // Clear buffer after each read to prevent data leakage
        if (bytesRead < static_cast<std::streamsize>(BUFFER_SIZE)) {
            std::memset(buffer + bytesRead, 0, BUFFER_SIZE - static_cast<size_t>(bytesRead));
        }
    }
    
    hasher_->finalize();
}

std::string StreamProcessor::getHash() const {
    if (!hasher_) {
        throw std::runtime_error("No hash implementation available");
    }
    return hasher_->getHash();
}

void StreamProcessor::reset() {
    if (hasher_) {
        hasher_->reset();
    }
}

std::string StreamProcessor::getAlgorithmName() const {
    if (!hasher_) {
        return "unknown";
    }
    return hasher_->getAlgorithmName();
}

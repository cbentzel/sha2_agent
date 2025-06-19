#ifndef HASH_CONFIG_H
#define HASH_CONFIG_H

#include <cstddef>

namespace HashConfig {
    
    // Build configuration
    static constexpr bool ENABLE_SECURITY_CHECKS = true;
    static constexpr bool ENABLE_MEMORY_CLEARING = true;
    static constexpr bool ENABLE_BOUNDS_CHECKING = true;
    
    // Performance tuning
    static constexpr size_t DEFAULT_BUFFER_SIZE = 32768;  // 32KB
    static constexpr size_t MIN_BUFFER_SIZE = 1024;       // 1KB
    static constexpr size_t MAX_BUFFER_SIZE = 1048576;    // 1MB
    
    // Security configuration
    static constexpr bool CLEAR_SENSITIVE_DATA = true;
    static constexpr bool VALIDATE_INPUT_SIZES = true;
    static constexpr bool STRICT_ERROR_HANDLING = true;
    
    // Version information
    static constexpr const char* VERSION = "1.0.0";
    static constexpr const char* BUILD_DATE = __DATE__;
    static constexpr const char* BUILD_TIME = __TIME__;
}

#endif // HASH_CONFIG_H

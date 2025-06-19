#ifndef HASH_LOGGING_H
#define HASH_LOGGING_H

#include <iostream>
#include <string>
#include <sstream>

namespace HashLogging {
    
    enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        NONE = 4
    };
    
    // Global log level - can be set at runtime
    extern LogLevel currentLogLevel;
    
    /**
     * Simple logging function
     */
    template<typename... Args>
    void log(LogLevel level, const std::string& category, Args&&... args) {
        if (level >= currentLogLevel) {
            const char* levelStr[] = {"DEBUG", "INFO", "WARN", "ERROR"};
            std::ostringstream oss;
            ((oss << args), ...);
            std::cerr << "[" << levelStr[level] << "] " << category << ": " << oss.str() << std::endl;
        }
    }
    
    // Convenience macros for different log levels
    #define LOG_DEBUG(category, ...) HashLogging::log(HashLogging::DEBUG, category, __VA_ARGS__)
    #define LOG_INFO(category, ...) HashLogging::log(HashLogging::INFO, category, __VA_ARGS__)
    #define LOG_WARNING(category, ...) HashLogging::log(HashLogging::WARNING, category, __VA_ARGS__)
    #define LOG_ERROR(category, ...) HashLogging::log(HashLogging::ERROR, category, __VA_ARGS__)
}

#endif // HASH_LOGGING_H

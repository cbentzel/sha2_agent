#include <iostream>
#include <iomanip>
#include "../src/blake256.h"

int main() {
    std::cout << "=== Testing BLAKE256 Implementation ===" << std::endl;
    
    // Test with explicit empty input
    BLAKE256 blake;
    std::cout << "Initial state - no input added" << std::endl;
    blake.finalize();
    std::string hash = blake.getHash();
    
    std::cout << "Our empty string result: " << hash << std::endl;
    std::cout << "Reference (from GitHub):  0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87" << std::endl;
    
    // Test with single zero byte (which is what the reference tests)
    BLAKE256 blake_zero;
    uint8_t zero_byte = 0;
    blake_zero.update(&zero_byte, 1);
    blake_zero.finalize();
    std::string hash_zero = blake_zero.getHash();
    
    std::cout << "Our 1-byte zero result:  " << hash_zero << std::endl;
    std::cout << "This should match ref:    0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87" << std::endl;
    
    return 0;
}

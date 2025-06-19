#include <iostream>
#include <iomanip>
#include "../src/blake256.h"

int main() {
    // Test empty string with our current implementation
    BLAKE256 blake;
    blake.finalize();
    std::string hash = blake.getHash();
    
    std::cout << "Our implementation for empty string: " << hash << std::endl;
    std::cout << "Expected from our tests:             716f6e863f744b9ac22c97ec7b76ea5f5908bc5b2f67c61510bfc4751384ea7a" << std::endl;
    std::cout << "Expected from reference:             0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87" << std::endl;
    
    // Test "abc"
    BLAKE256 blake2;
    std::string input = "abc";
    blake2.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake2.finalize();
    std::string hash2 = blake2.getHash();
    
    std::cout << std::endl;
    std::cout << "Our implementation for 'abc':       " << hash2 << std::endl;
    std::cout << "Expected from our tests:             bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319" << std::endl;
    
    return 0;
}

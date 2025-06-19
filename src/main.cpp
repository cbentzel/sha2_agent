#include "sha256.h"
#include <iostream>

int main() {
    SHA256 hasher;
    
    // Process input from stdin
    hasher.processStream(std::cin);
    
    // Output the hash to stdout
    std::cout << hasher.getHash() << std::endl;
    
    return 0;
}

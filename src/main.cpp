#include "stream_processor.h"
#include "hash_factory.h"
#include <iostream>
#include <string>
#include <cstring>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " --algorithm=<hash_type>\n\n";
    std::cout << "Hash calculator that reads from stdin and outputs hash to stdout.\n\n";
    std::cout << "Options:\n";
    std::cout << "  --algorithm=<type>  Hash algorithm to use\n";
    std::cout << "  -a <type>           Short form of --algorithm\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --list              List supported algorithms\n\n";
    std::cout << "Supported algorithms:\n";
    auto algorithms = HashFactory::getSupportedAlgorithms();
    for (const auto& algo : algorithms) {
        std::cout << "  " << algo << "\n";
    }
    std::cout << "\nExamples:\n";
    std::cout << "  echo -n 'hello world' | " << programName << " --algorithm=sha256\n";
    std::cout << "  " << programName << " -a md5 < file.txt\n";
}

void printSupportedAlgorithms() {
    auto algorithms = HashFactory::getSupportedAlgorithms();
    for (const auto& algo : algorithms) {
        std::cout << algo << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::string algorithm;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--list") {
            printSupportedAlgorithms();
            return 0;
        } else if (arg.substr(0, 12) == "--algorithm=") {
            algorithm = arg.substr(12);
        } else if (arg == "-a" && i + 1 < argc) {
            algorithm = argv[++i];
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (algorithm.empty()) {
        std::cerr << "Error: No algorithm specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        // Create hash implementation
        auto hasher = HashFactory::createHash(algorithm);
        
        // Create stream processor
        StreamProcessor processor(std::move(hasher));
        
        // Process input from stdin
        processor.processStream(std::cin);
        
        // Output the hash to stdout
        std::cout << processor.getHash() << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

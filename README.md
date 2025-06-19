# Generic Hash Calculator (hashgen)

A lightweight command-line utility that calculates cryptographic hashes using multiple algorithms from standard input.

## Overview

This utility implements multiple cryptographic hash functions with a unified interface. It reads data from standard input and outputs the resulting hash in hexadecimal format to standard output. The program supports MD5, SHA-1, and SHA-256 algorithms with a common streaming architecture.

## Features

- **Multiple Hash Algorithms**: MD5, SHA-1, SHA-256 with unified interface
- **Stream Processing**: Efficiently processes large files using a 32KB internal buffer
- **Standard Compliance**: Implements algorithms according to FIPS 180-4 and RFC 1321
- **Memory Efficient**: Uses minimal memory regardless of input size
- **Security Hardened**: Includes overflow protection and secure memory handling
- **Cross-Platform**: Written in standard C++14 with no external dependencies

## Supported Algorithms

| Algorithm | Output Size | Standard | Description |
|-----------|-------------|----------|-------------|
| MD5       | 128 bits (32 hex chars) | RFC 1321 | MD5 Message-Digest Algorithm |
| SHA1      | 160 bits (40 hex chars) | FIPS 180-4 | Secure Hash Algorithm 1 |
| SHA256    | 256 bits (64 hex chars) | FIPS 180-4 | Secure Hash Algorithm 256 |
| SHA128    | 160 bits (40 hex chars) | - | Alias for SHA-1 |

## Building

### Prerequisites

- CMake 3.10 or higher
- C++14 compatible compiler (GCC, Clang, or MSVC)
- Google Test (optional, for running unit tests)

### On macOS

```bash
# Install dependencies (optional, for tests)
brew install googletest cmake

# Clone and build
git clone <repository-url>
cd sha2_agent
mkdir build && cd build
cmake ..
make
```

### Build Commands

```bash
# Basic build
mkdir build && cd build
cmake ..
make

# Build with tests (requires Google Test)
mkdir build && cd build
cmake ..
make
make test
```

## Usage

### Command Line Options

```bash
hashgen --algorithm=<ALGORITHM>
hashgen -a <ALGORITHM>
hashgen --help
hashgen --list
```

**Options:**
- `--algorithm=<type>` : Specify hash algorithm (required)
- `-a <type>` : Short form of --algorithm
- `--help, -h` : Show help message
- `--list` : List supported algorithms

### Examples

```bash
# Calculate SHA256 hash of a file
hashgen --algorithm=sha256 < input.txt

# Calculate MD5 hash of a string
echo -n "hello world" | hashgen -a md5

# Calculate SHA-1 hash of command output
ls -la | hashgen --algorithm=sha1

# List supported algorithms
hashgen --list

# Get help
hashgen --help
```

### Known Test Vectors

```bash
# Empty string hashes
echo -n "" | hashgen -a md5      # d41d8cd98f00b204e9800998ecf8427e
echo -n "" | hashgen -a sha1     # da39a3ee5e6b4b0d3255bfef95601890afd80709
echo -n "" | hashgen -a sha256   # e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

# "abc" hashes
echo -n "abc" | hashgen -a md5    # 900150983cd24fb0d6963f7d28e17f72
echo -n "abc" | hashgen -a sha1   # a9993e364706816aba3e25717850c26c9cd0d89d
echo -n "abc" | hashgen -a sha256 # ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

## Architecture

### Design Principles

The implementation follows object-oriented design principles with:

- **Abstract Base Class**: `HashInterface` defines common interface
- **Stream Processor**: `StreamProcessor` handles I/O operations  
- **Concrete Implementations**: Algorithm-specific classes (SHA256, SHA1, MD5)
- **Factory Pattern**: `HashFactory` creates algorithm instances
- **Security First**: Bounds checking and secure memory handling throughout

### Class Hierarchy

```
HashInterface (abstract)
├── SHA256
├── SHA1  
└── MD5

StreamProcessor (composition with HashInterface)
HashFactory (static factory methods)
```

### Memory Usage

- **Maximum internal buffer**: 32KB for streaming large inputs
- **Working memory per algorithm**: ~1-2KB for algorithm state
- **Total memory footprint**: <100KB regardless of input size

## Testing

The project includes comprehensive unit tests using Google Test:

```bash
# Run all tests
make test

# Run tests with verbose output
./hash_tests --gtest_verbose
```

### Test Coverage

- **Factory Pattern**: Algorithm creation and validation
- **Empty inputs**: All algorithms with empty string
- **Standard test vectors**: Known hash values from specifications
- **Large inputs**: >32KB streaming test
- **Binary data**: All byte values (0-255)
- **Error handling**: Invalid algorithms, edge cases
- **Reset functionality**: Hasher reuse validation

## Performance

- **Throughput**: Typically 100-500 MB/s depending on hardware and algorithm
- **Memory**: Constant O(1) memory usage regardless of input size
- **Streaming**: Processes arbitrarily large files without loading into memory

### Algorithm Performance

| Algorithm | Relative Speed | Security Level |
|-----------|----------------|----------------|
| MD5       | Fastest        | Deprecated (cryptographically broken) |
| SHA-1     | Fast           | Deprecated (theoretical attacks exist) |
| SHA-256   | Moderate       | Secure (recommended) |

## Installation

```bash
# Install to system directories (requires sudo on macOS/Linux)
make install

# This installs:
# - Binary: /usr/local/bin/hashgen
# - Man page: /usr/local/share/man/man1/hashgen.1
```

## Compatibility

- **C++ Standard**: Requires C++14 or later
- **Platforms**: macOS, Linux, Windows (with appropriate compiler)
- **Dependencies**: None (standard library only)
- **Testing**: Google Test framework (optional)

## Security Considerations

### Implemented Protections

- **Buffer overflow protection**: Bounds checking on all buffer operations
- **Integer overflow detection**: Prevents silent wraparound in length calculations
- **Memory initialization**: Prevents information leakage through uninitialized memory
- **Secure memory clearing**: Sensitive data is cleared after use
- **Input validation**: Proper error handling for invalid inputs

### Algorithm Security Status

- **MD5**: ⚠️ **DEPRECATED** - Cryptographically broken, use only for non-security applications
- **SHA-1**: ⚠️ **DEPRECATED** - Theoretical attacks exist, avoid for new applications  
- **SHA-256**: ✅ **SECURE** - Currently recommended for cryptographic use

## Technical Implementation

### Core Components

- **HashInterface**: Abstract base defining common hash operations
- **StreamProcessor**: Unified streaming I/O handler
- **Algorithm Classes**: Concrete implementations for each hash function
- **HashFactory**: Factory for creating algorithm instances

### Key Features

- **RAII**: Automatic resource management with smart pointers
- **Exception Safety**: Comprehensive error handling
- **Standard Compliance**: Follows C++ best practices
- **Const-correctness**: Proper const method marking

## Future Enhancements

Planned additions include:
- SHA-512 implementation
- BLAKE2b and BLAKE2s support  
- HMAC variants
- Multi-threading support for large files

## Verification

Test vectors from official standards are included in the unit tests to verify correctness against:
- FIPS 180-4 (SHA algorithms)
- RFC 1321 (MD5)
- Known test vector databases

## License

This implementation is provided as-is for educational and practical use.

## Contributing

Contributions are welcome. Please ensure all tests pass and add appropriate test coverage for new features.

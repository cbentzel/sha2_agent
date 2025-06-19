#include <gtest/gtest.h>
#include "md5.h"
#include "stream_processor.h"
#include <sstream>
#include <memory>

class MD5Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh setup for each test
    }
};

// Test empty string
TEST_F(MD5Test, EmptyString) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    // MD5 of empty string should be d41d8cd98f00b204e9800998ecf8427e
    EXPECT_EQ(processor.getHash(), "d41d8cd98f00b204e9800998ecf8427e");
}

// Test single character
TEST_F(MD5Test, SingleCharacter) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("a");
    processor.processStream(input);
    
    // MD5 of "a" should be 0cc175b9c0f1b6a831c399e269772661
    EXPECT_EQ(processor.getHash(), "0cc175b9c0f1b6a831c399e269772661");
}

// Test "abc"
TEST_F(MD5Test, ABC) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    // MD5 of "abc" should be 900150983cd24fb0d6963f7d28e17f72
    EXPECT_EQ(processor.getHash(), "900150983cd24fb0d6963f7d28e17f72");
}

// Test "message digest"
TEST_F(MD5Test, MessageDigest) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("message digest");
    processor.processStream(input);
    
    // MD5 of "message digest" should be f96b697d7cb7938d525a2f31aaf161d0
    EXPECT_EQ(processor.getHash(), "f96b697d7cb7938d525a2f31aaf161d0");
}

// Test alphabet
TEST_F(MD5Test, Alphabet) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abcdefghijklmnopqrstuvwxyz");
    processor.processStream(input);
    
    // MD5 of "abcdefghijklmnopqrstuvwxyz" should be c3fcd3d76192e4007dfb496cca67e13b
    EXPECT_EQ(processor.getHash(), "c3fcd3d76192e4007dfb496cca67e13b");
}

// Test alphanumeric
TEST_F(MD5Test, Alphanumeric) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    processor.processStream(input);
    
    // MD5 of alphanumeric string should be d174ab98d277d9f5a5611c2c9f419d9f
    EXPECT_EQ(processor.getHash(), "d174ab98d277d9f5a5611c2c9f419d9f");
}

// Test longer string (80 characters)
TEST_F(MD5Test, EightyCharacters) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    processor.processStream(input);
    
    // MD5 of 80 '0-9' repeated digits should be 57edf4a22be3c955ac49da2e2107b67a
    EXPECT_EQ(processor.getHash(), "57edf4a22be3c955ac49da2e2107b67a");
}

// Test reset functionality
TEST_F(MD5Test, ResetFunctionality) {
    auto hasher1 = std::make_unique<MD5>();
    StreamProcessor processor1(std::move(hasher1));
    
    std::istringstream input1("abc");
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    
    auto hasher2 = std::make_unique<MD5>();
    StreamProcessor processor2(std::move(hasher2));
    
    std::istringstream input2("abc");
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    
    EXPECT_EQ(hash1, hash2);
}

// Test binary data
TEST_F(MD5Test, BinaryData) {
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(binaryData);
    processor.processStream(input);
    
    // This should produce a consistent hash for the binary sequence 0-255
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 32); // MD5 hash should be 32 hex characters
    
    // Test that it's reproducible
    auto hasher2 = std::make_unique<MD5>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(binaryData);
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test large data streaming (simulate larger than buffer size)
TEST_F(MD5Test, LargeDataStreaming) {
    std::ostringstream oss;
    // Create data larger than our 32K buffer
    for (int i = 0; i < 40000; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 32); // Should be 32 hex characters
    
    // Test reproducibility
    auto hasher2 = std::make_unique<MD5>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test block boundary conditions
TEST_F(MD5Test, BlockBoundaries) {
    // Test data that's exactly one block (64 bytes)
    std::string oneBlock(64, 'X');
    auto hasher1 = std::make_unique<MD5>();
    StreamProcessor processor1(std::move(hasher1));
    std::istringstream input1(oneBlock);
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    EXPECT_EQ(hash1.length(), 32);
    
    // Test data that's one byte less than one block
    std::string almostOneBlock(63, 'X');
    auto hasher2 = std::make_unique<MD5>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(almostOneBlock);
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    EXPECT_EQ(hash2.length(), 32);
    EXPECT_NE(hash1, hash2); // Should be different
    
    // Test data that's one byte more than one block
    std::string overOneBlock(65, 'X');
    auto hasher3 = std::make_unique<MD5>();
    StreamProcessor processor3(std::move(hasher3));
    std::istringstream input3(overOneBlock);
    processor3.processStream(input3);
    std::string hash3 = processor3.getHash();
    EXPECT_EQ(hash3.length(), 32);
    EXPECT_NE(hash1, hash3); // Should be different
    EXPECT_NE(hash2, hash3); // Should be different
}

// Test very large input (stress test)
TEST_F(MD5Test, VeryLargeInput) {
    std::ostringstream oss;
    // Create 1MB of data
    for (int i = 0; i < 1048576; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 32);
    
    // Test reproducibility with large data
    auto hasher2 = std::make_unique<MD5>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test algorithm properties
TEST_F(MD5Test, AlgorithmProperties) {
    auto hasher = std::make_unique<MD5>();
    EXPECT_EQ(hasher->getAlgorithmName(), "MD5");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 16);
}

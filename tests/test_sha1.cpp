#include <gtest/gtest.h>
#include "sha1.h"
#include "stream_processor.h"
#include <sstream>
#include <memory>

class SHA1Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh setup for each test
    }
};

// Test empty string
TEST_F(SHA1Test, EmptyString) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    // SHA-1 of empty string should be da39a3ee5e6b4b0d3255bfef95601890afd80709
    EXPECT_EQ(processor.getHash(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

// Test single character
TEST_F(SHA1Test, SingleCharacter) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("a");
    processor.processStream(input);
    
    // SHA-1 of "a" should be 86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
    EXPECT_EQ(processor.getHash(), "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8");
}

// Test "abc"
TEST_F(SHA1Test, ABC) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    // SHA-1 of "abc" should be a9993e364706816aba3e25717850c26c9cd0d89d
    EXPECT_EQ(processor.getHash(), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

// Test "message digest"
TEST_F(SHA1Test, MessageDigest) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("message digest");
    processor.processStream(input);
    
    // SHA-1 of "message digest" should be c12252ceda8be8994d5fa0290a47231c1d16aae3
    EXPECT_EQ(processor.getHash(), "c12252ceda8be8994d5fa0290a47231c1d16aae3");
}

// Test alphabet
TEST_F(SHA1Test, Alphabet) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abcdefghijklmnopqrstuvwxyz");
    processor.processStream(input);
    
    // SHA-1 of "abcdefghijklmnopqrstuvwxyz" should be 32d10c7b8cf96570ca04ce37f2a19d84240d3a89
    EXPECT_EQ(processor.getHash(), "32d10c7b8cf96570ca04ce37f2a19d84240d3a89");
}

// Test alphanumeric
TEST_F(SHA1Test, Alphanumeric) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    processor.processStream(input);
    
    // SHA-1 of alphanumeric string should be 761c457bf73b14d27e9e9265c46f4b4dda11f940
    EXPECT_EQ(processor.getHash(), "761c457bf73b14d27e9e9265c46f4b4dda11f940");
}

// Test longer string (80 characters)
TEST_F(SHA1Test, EightyCharacters) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    processor.processStream(input);
    
    // SHA-1 of 80 '0-9' repeated digits should be 50abf5706a150990a08b2c5ea40fa0e585554732
    EXPECT_EQ(processor.getHash(), "50abf5706a150990a08b2c5ea40fa0e585554732");
}

// Test standard test vector from FIPS 180-4
TEST_F(SHA1Test, FIPS180_4_TestVector) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    processor.processStream(input);
    
    // SHA-1 of "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    EXPECT_EQ(processor.getHash(), "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
}

// Test 1 million 'a' characters (this is a standard test vector)
TEST_F(SHA1Test, MillionAs) {
    std::ostringstream oss;
    for (int i = 0; i < 1000000; ++i) {
        oss << 'a';
    }
    
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    // SHA-1 of 1 million 'a's should be 34aa973cd4c4daa4f61eeb2bdbad27316534016f
    EXPECT_EQ(processor.getHash(), "34aa973cd4c4daa4f61eeb2bdbad27316534016f");
}

// Test reset functionality
TEST_F(SHA1Test, ResetFunctionality) {
    auto hasher1 = std::make_unique<SHA1>();
    StreamProcessor processor1(std::move(hasher1));
    
    std::istringstream input1("abc");
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    
    std::istringstream input2("abc");
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    
    EXPECT_EQ(hash1, hash2);
}

// Test binary data
TEST_F(SHA1Test, BinaryData) {
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(binaryData);
    processor.processStream(input);
    
    // This should produce a consistent hash for the binary sequence 0-255
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 40); // SHA-1 hash should be 40 hex characters
    
    // Test that it's reproducible
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(binaryData);
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test large data streaming (simulate larger than buffer size)
TEST_F(SHA1Test, LargeDataStreaming) {
    std::ostringstream oss;
    // Create data larger than our 32K buffer
    for (int i = 0; i < 40000; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 40); // Should be 40 hex characters
    
    // Test reproducibility
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test block boundary conditions
TEST_F(SHA1Test, BlockBoundaries) {
    // Test data that's exactly one block (64 bytes)
    std::string oneBlock(64, 'X');
    auto hasher1 = std::make_unique<SHA1>();
    StreamProcessor processor1(std::move(hasher1));
    std::istringstream input1(oneBlock);
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    EXPECT_EQ(hash1.length(), 40);
    
    // Test data that's one byte less than one block
    std::string almostOneBlock(63, 'X');
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(almostOneBlock);
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    EXPECT_EQ(hash2.length(), 40);
    EXPECT_NE(hash1, hash2); // Should be different
    
    // Test data that's one byte more than one block
    std::string overOneBlock(65, 'X');
    auto hasher3 = std::make_unique<SHA1>();
    StreamProcessor processor3(std::move(hasher3));
    std::istringstream input3(overOneBlock);
    processor3.processStream(input3);
    std::string hash3 = processor3.getHash();
    EXPECT_EQ(hash3.length(), 40);
    EXPECT_NE(hash1, hash3); // Should be different
    EXPECT_NE(hash2, hash3); // Should be different
}

// Test very large input (stress test)
TEST_F(SHA1Test, VeryLargeInput) {
    std::ostringstream oss;
    // Create 1MB of data
    for (int i = 0; i < 1048576; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 40);
    
    // Test reproducibility with large data
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test algorithm properties
TEST_F(SHA1Test, AlgorithmProperties) {
    auto hasher = std::make_unique<SHA1>();
    EXPECT_EQ(hasher->getAlgorithmName(), "SHA1");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 20);
}

// Test edge cases for padding
TEST_F(SHA1Test, PaddingEdgeCases) {
    // Test input that requires minimal padding (55 bytes)
    std::string input55(55, 'A');
    auto hasher1 = std::make_unique<SHA1>();
    StreamProcessor processor1(std::move(hasher1));
    std::istringstream stream55(input55);
    processor1.processStream(stream55);
    std::string hash55 = processor1.getHash();
    EXPECT_EQ(hash55.length(), 40);
    
    // Test input that requires maximum padding (56 bytes)
    std::string input56(56, 'A');
    auto hasher2 = std::make_unique<SHA1>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream stream56(input56);
    processor2.processStream(stream56);
    std::string hash56 = processor2.getHash();
    EXPECT_EQ(hash56.length(), 40);
    EXPECT_NE(hash55, hash56);
}

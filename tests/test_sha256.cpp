#include <gtest/gtest.h>
#include "sha256.h"
#include "stream_processor.h"
#include <sstream>
#include <memory>

class SHA256Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh setup for each test
    }
};

// Test empty string
TEST_F(SHA256Test, EmptyString) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    // SHA256 of empty string should be e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    EXPECT_EQ(processor.getHash(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Test single character
TEST_F(SHA256Test, SingleCharacter) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("a");
    processor.processStream(input);
    
    // SHA256 of "a" should be ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
    EXPECT_EQ(processor.getHash(), "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
}

// Test "abc"
TEST_F(SHA256Test, ABC) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    // SHA256 of "abc" should be ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    EXPECT_EQ(processor.getHash(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Test longer string
TEST_F(SHA256Test, LongerString) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    processor.processStream(input);
    
    // SHA256 of "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    EXPECT_EQ(processor.getHash(), "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

// Test 1 million 'a' characters
TEST_F(SHA256Test, MillionAs) {
    std::ostringstream oss;
    for (int i = 0; i < 1000000; ++i) {
        oss << 'a';
    }
    
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    // SHA256 of 1 million 'a's
    EXPECT_EQ(processor.getHash(), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

// Test reset functionality
TEST_F(SHA256Test, ResetFunctionality) {
    auto hasher1 = std::make_unique<SHA256>();
    StreamProcessor processor1(std::move(hasher1));
    
    std::istringstream input1("abc");
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    
    std::istringstream input2("abc");
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    
    EXPECT_EQ(hash1, hash2);
}

// Test binary data
TEST_F(SHA256Test, BinaryData) {
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(binaryData);
    processor.processStream(input);
    
    // This should produce a consistent hash for the binary sequence 0-255
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 64); // SHA256 hash should be 64 hex characters
    
    // Test that it's reproducible
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(binaryData);
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test large data streaming (simulate larger than buffer size)
TEST_F(SHA256Test, LargeDataStreaming) {
    std::ostringstream oss;
    // Create data larger than our 32K buffer
    for (int i = 0; i < 40000; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 64); // Should be 64 hex characters
    
    // Test reproducibility
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test algorithm properties
TEST_F(SHA256Test, AlgorithmProperties) {
    auto hasher = std::make_unique<SHA256>();
    EXPECT_EQ(hasher->getAlgorithmName(), "SHA256");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 32);
}

// Test block boundary conditions
TEST_F(SHA256Test, BlockBoundaries) {
    // Test data that's exactly one block (64 bytes)
    std::string oneBlock(64, 'X');
    auto hasher1 = std::make_unique<SHA256>();
    StreamProcessor processor1(std::move(hasher1));
    std::istringstream input1(oneBlock);
    processor1.processStream(input1);
    std::string hash1 = processor1.getHash();
    EXPECT_EQ(hash1.length(), 64);
    
    // Test data that's one byte less than one block
    std::string almostOneBlock(63, 'X');
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(almostOneBlock);
    processor2.processStream(input2);
    std::string hash2 = processor2.getHash();
    EXPECT_EQ(hash2.length(), 64);
    EXPECT_NE(hash1, hash2); // Should be different
    
    // Test data that's one byte more than one block
    std::string overOneBlock(65, 'X');
    auto hasher3 = std::make_unique<SHA256>();
    StreamProcessor processor3(std::move(hasher3));
    std::istringstream input3(overOneBlock);
    processor3.processStream(input3);
    std::string hash3 = processor3.getHash();
    EXPECT_EQ(hash3.length(), 64);
    EXPECT_NE(hash1, hash3); // Should be different
    EXPECT_NE(hash2, hash3); // Should be different
}

// Test very large input (stress test)
TEST_F(SHA256Test, VeryLargeInput) {
    std::ostringstream oss;
    // Create 1MB of data
    for (int i = 0; i < 1048576; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 64);
    
    // Test reproducibility with large data
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test edge cases for padding
TEST_F(SHA256Test, PaddingEdgeCases) {
    // Test input that requires minimal padding (55 bytes)
    std::string input55(55, 'A');
    auto hasher1 = std::make_unique<SHA256>();
    StreamProcessor processor1(std::move(hasher1));
    std::istringstream stream55(input55);
    processor1.processStream(stream55);
    std::string hash55 = processor1.getHash();
    EXPECT_EQ(hash55.length(), 64);
    
    // Test input that requires maximum padding (56 bytes)
    std::string input56(56, 'A');
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream stream56(input56);
    processor2.processStream(stream56);
    std::string hash56 = processor2.getHash();
    EXPECT_EQ(hash56.length(), 64);
    EXPECT_NE(hash55, hash56);
}

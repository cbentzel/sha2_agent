#include <gtest/gtest.h>
#include "sha256.h"
#include <sstream>

class SHA256Test : public ::testing::Test {
protected:
    SHA256 hasher;
    
    void SetUp() override {
        hasher.reset();
    }
};

// Test empty string
TEST_F(SHA256Test, EmptyString) {
    std::istringstream input("");
    hasher.processStream(input);
    
    // SHA256 of empty string should be e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    EXPECT_EQ(hasher.getHash(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// Test single character
TEST_F(SHA256Test, SingleCharacter) {
    std::istringstream input("a");
    hasher.processStream(input);
    
    // SHA256 of "a" should be ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
    EXPECT_EQ(hasher.getHash(), "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb");
}

// Test "abc"
TEST_F(SHA256Test, ABC) {
    std::istringstream input("abc");
    hasher.processStream(input);
    
    // SHA256 of "abc" should be ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    EXPECT_EQ(hasher.getHash(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Test longer string
TEST_F(SHA256Test, LongerString) {
    std::istringstream input("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
    hasher.processStream(input);
    
    // SHA256 of "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    EXPECT_EQ(hasher.getHash(), "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

// Test 1 million 'a' characters
TEST_F(SHA256Test, MillionAs) {
    std::ostringstream oss;
    for (int i = 0; i < 1000000; ++i) {
        oss << 'a';
    }
    std::istringstream input(oss.str());
    hasher.processStream(input);
    
    // SHA256 of 1 million 'a's
    EXPECT_EQ(hasher.getHash(), "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

// Test reset functionality
TEST_F(SHA256Test, ResetFunctionality) {
    std::istringstream input1("abc");
    hasher.processStream(input1);
    std::string hash1 = hasher.getHash();
    
    hasher.reset();
    
    std::istringstream input2("abc");
    hasher.processStream(input2);
    std::string hash2 = hasher.getHash();
    
    EXPECT_EQ(hash1, hash2);
}

// Test binary data
TEST_F(SHA256Test, BinaryData) {
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    
    std::istringstream input(binaryData);
    hasher.processStream(input);
    
    // This should produce a consistent hash for the binary sequence 0-255
    std::string hash = hasher.getHash();
    EXPECT_EQ(hash.length(), 64); // SHA256 hash should be 64 hex characters
    
    // Test that it's reproducible
    hasher.reset();
    std::istringstream input2(binaryData);
    hasher.processStream(input2);
    EXPECT_EQ(hasher.getHash(), hash);
}

// Test large data streaming (simulate larger than buffer size)
TEST_F(SHA256Test, LargeDataStreaming) {
    std::ostringstream oss;
    // Create data larger than our 32K buffer
    for (int i = 0; i < 40000; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    std::istringstream input(oss.str());
    hasher.processStream(input);
    
    std::string hash = hasher.getHash();
    EXPECT_EQ(hash.length(), 64); // Should be 64 hex characters
    
    // Test reproducibility
    hasher.reset();
    std::istringstream input2(oss.str());
    hasher.processStream(input2);
    EXPECT_EQ(hasher.getHash(), hash);
}

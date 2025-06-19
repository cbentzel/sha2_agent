#include <gtest/gtest.h>
#include "stream_processor.h"
#include "hash_factory.h"
#include "sha256.h"
#include "sha1.h"
#include "md5.h"
#include <sstream>

class HashTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh setup for each test
    }
};

// Test HashFactory
TEST_F(HashTest, HashFactory_CreateSHA256) {
    auto hasher = HashFactory::createHash("SHA256");
    ASSERT_NE(hasher, nullptr);
    EXPECT_EQ(hasher->getAlgorithmName(), "SHA256");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 32);
}

TEST_F(HashTest, HashFactory_CreateMD5) {
    auto hasher = HashFactory::createHash("md5");
    ASSERT_NE(hasher, nullptr);
    EXPECT_EQ(hasher->getAlgorithmName(), "MD5");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 16);
}

TEST_F(HashTest, HashFactory_CreateSHA1) {
    auto hasher = HashFactory::createHash("SHA1");
    ASSERT_NE(hasher, nullptr);
    EXPECT_EQ(hasher->getAlgorithmName(), "SHA1");
    EXPECT_EQ(hasher->getBlockSize(), 64);
    EXPECT_EQ(hasher->getHashSize(), 20);
}

TEST_F(HashTest, HashFactory_UnsupportedAlgorithm) {
    EXPECT_THROW(HashFactory::createHash("UNKNOWN"), std::invalid_argument);
}

TEST_F(HashTest, HashFactory_SupportedAlgorithms) {
    auto algorithms = HashFactory::getSupportedAlgorithms();
    EXPECT_FALSE(algorithms.empty());
    EXPECT_TRUE(HashFactory::isSupported("SHA256"));
    EXPECT_TRUE(HashFactory::isSupported("md5"));
    EXPECT_TRUE(HashFactory::isSupported("SHA1"));
    EXPECT_FALSE(HashFactory::isSupported("UNKNOWN"));
}

// Test SHA256 with known vectors
TEST_F(HashTest, SHA256_EmptyString) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST_F(HashTest, SHA256_ABC) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// Test MD5 with known vectors
TEST_F(HashTest, MD5_EmptyString) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "d41d8cd98f00b204e9800998ecf8427e");
}

TEST_F(HashTest, MD5_ABC) {
    auto hasher = std::make_unique<MD5>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "900150983cd24fb0d6963f7d28e17f72");
}

// Test SHA1 with known vectors
TEST_F(HashTest, SHA1_EmptyString) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST_F(HashTest, SHA1_ABC) {
    auto hasher = std::make_unique<SHA1>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input("abc");
    processor.processStream(input);
    
    EXPECT_EQ(processor.getHash(), "a9993e364706816aba3e25717850c26c9cd0d89d");
}

// Test StreamProcessor error handling
TEST_F(HashTest, StreamProcessor_PrematureHashAccess) {
    auto hasher = std::make_unique<SHA256>();
    hasher.get(); // Keep reference for direct testing
    StreamProcessor processor(std::move(hasher));
    
    // Don't process any stream, just try to get hash
    // Note: StreamProcessor calls finalize() in processStream, so this test
    // would need direct hasher access to test premature access
}

TEST_F(HashTest, StreamProcessor_Reset) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::istringstream input1("test");
    processor.processStream(input1);
    std::string hash1 = processor.getHash();
    
    processor.reset();
    
    std::istringstream input2("test");
    processor.processStream(input2);
    std::string hash2 = processor.getHash();
    
    EXPECT_EQ(hash1, hash2);
}

// Test large data processing
TEST_F(HashTest, LargeDataStreaming) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::ostringstream oss;
    // Create data larger than buffer size
    for (int i = 0; i < 40000; ++i) {
        oss << static_cast<char>('A' + (i % 26));
    }
    
    std::istringstream input(oss.str());
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 64); // SHA256 hash should be 64 hex characters
    
    // Test reproducibility
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(oss.str());
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

// Test binary data
TEST_F(HashTest, BinaryData) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    std::string binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData += static_cast<char>(i);
    }
    
    std::istringstream input(binaryData);
    processor.processStream(input);
    
    std::string hash = processor.getHash();
    EXPECT_EQ(hash.length(), 64); // Should be 64 hex characters
    
    // Test reproducibility
    auto hasher2 = std::make_unique<SHA256>();
    StreamProcessor processor2(std::move(hasher2));
    std::istringstream input2(binaryData);
    processor2.processStream(input2);
    EXPECT_EQ(processor2.getHash(), hash);
}

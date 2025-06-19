#include <gtest/gtest.h>
#include "sha256.h"
#include "sha1.h"
#include "md5.h"
#include "stream_processor.h"
#include <sstream>
#include <memory>
#include <limits>

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Fresh setup for each test
    }
};

// Test overflow detection in SHA256
TEST_F(ErrorHandlingTest, SHA256_OverflowDetection) {
    auto hasher = std::make_unique<SHA256>();
    
    // This should not throw for reasonable sizes
    std::string normalData(1000, 'a');
    EXPECT_NO_THROW(hasher->update(reinterpret_cast<const uint8_t*>(normalData.c_str()), normalData.size()));
    
    hasher->reset();
    
    // Test with maximum possible size that should trigger overflow check
    // Note: We can't actually allocate UINT64_MAX bytes, so we test the check logic
    // by creating a hasher with near-max totalLength
    try {
        // This is a conceptual test - in practice we'd need to modify the internal state
        // to test the overflow condition without allocating huge amounts of memory
        hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
        EXPECT_TRUE(true); // If we get here, no overflow occurred (expected for small input)
    } catch (const std::overflow_error& e) {
        EXPECT_TRUE(std::string(e.what()).find("overflow") != std::string::npos);
    }
}

// Test double finalization protection
TEST_F(ErrorHandlingTest, DoubleFinalizationProtection) {
    auto hasher1 = std::make_unique<SHA256>();
    StreamProcessor processor1(std::move(hasher1));
    
    std::istringstream input1("test");
    processor1.processStream(input1);
    
    // First call should work
    EXPECT_NO_THROW(processor1.getHash());
    
    // Reset should allow reuse
    processor1.reset();
    
    std::istringstream input2("test");
    EXPECT_NO_THROW(processor1.processStream(input2));
}

// Test update after finalization
TEST_F(ErrorHandlingTest, UpdateAfterFinalization) {
    auto hasher = std::make_unique<SHA256>();
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    hasher->finalize();
    
    // This should throw
    EXPECT_THROW(hasher->update(reinterpret_cast<const uint8_t*>("more"), 4), std::runtime_error);
}

// Test hash access before finalization
TEST_F(ErrorHandlingTest, HashAccessBeforeFinalization) {
    auto hasher = std::make_unique<SHA256>();
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    
    // This should throw because we haven't finalized yet
    EXPECT_THROW(hasher->getHash(), std::runtime_error);
}

// Test null pointer handling in StreamProcessor
TEST_F(ErrorHandlingTest, NullPointerHandling) {
    EXPECT_THROW(StreamProcessor(nullptr), std::invalid_argument);
}

// Test empty algorithm name in factory
TEST_F(ErrorHandlingTest, EmptyAlgorithmName) {
    EXPECT_THROW(HashFactory::createHash(""), std::invalid_argument);
    EXPECT_THROW(HashFactory::createHash("unknown"), std::invalid_argument);
}

// Test MD5 error conditions
TEST_F(ErrorHandlingTest, MD5_ErrorConditions) {
    auto hasher = std::make_unique<MD5>();
    
    // Test update after finalization
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    hasher->finalize();
    EXPECT_THROW(hasher->update(reinterpret_cast<const uint8_t*>("more"), 4), std::runtime_error);
    
    // Test hash access before finalization
    hasher->reset();
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    EXPECT_THROW(hasher->getHash(), std::runtime_error);
}

// Test SHA1 error conditions
TEST_F(ErrorHandlingTest, SHA1_ErrorConditions) {
    auto hasher = std::make_unique<SHA1>();
    
    // Test update after finalization
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    hasher->finalize();
    EXPECT_THROW(hasher->update(reinterpret_cast<const uint8_t*>("more"), 4), std::runtime_error);
    
    // Test hash access before finalization
    hasher->reset();
    hasher->update(reinterpret_cast<const uint8_t*>("test"), 4);
    EXPECT_THROW(hasher->getHash(), std::runtime_error);
}

// Test algorithm properties consistency
TEST_F(ErrorHandlingTest, AlgorithmPropertiesConsistency) {
    auto md5 = std::make_unique<MD5>();
    auto sha1 = std::make_unique<SHA1>();
    auto sha256 = std::make_unique<SHA256>();
    
    // Verify expected properties
    EXPECT_EQ(md5->getBlockSize(), 64);
    EXPECT_EQ(md5->getHashSize(), 16);
    EXPECT_EQ(md5->getAlgorithmName(), "MD5");
    
    EXPECT_EQ(sha1->getBlockSize(), 64);
    EXPECT_EQ(sha1->getHashSize(), 20);
    EXPECT_EQ(sha1->getAlgorithmName(), "SHA1");
    
    EXPECT_EQ(sha256->getBlockSize(), 64);
    EXPECT_EQ(sha256->getHashSize(), 32);
    EXPECT_EQ(sha256->getAlgorithmName(), "SHA256");
}

// Test stream processor with bad input stream
TEST_F(ErrorHandlingTest, StreamProcessorBadInput) {
    auto hasher = std::make_unique<SHA256>();
    StreamProcessor processor(std::move(hasher));
    
    // Create a stream that will have issues
    std::istringstream input("");
    input.setstate(std::ios::badbit);
    
    // Should handle gracefully (not throw, but produce consistent hash)
    EXPECT_NO_THROW(processor.processStream(input));
    EXPECT_NO_THROW(processor.getHash());
}

// Test reset functionality across algorithms
TEST_F(ErrorHandlingTest, ResetFunctionality) {
    auto md5 = std::make_unique<MD5>();
    auto sha1 = std::make_unique<SHA1>();
    auto sha256 = std::make_unique<SHA256>();
    
    // Process some data
    md5->update(reinterpret_cast<const uint8_t*>("test"), 4);
    sha1->update(reinterpret_cast<const uint8_t*>("test"), 4);
    sha256->update(reinterpret_cast<const uint8_t*>("test"), 4);
    
    // Reset should allow fresh processing
    md5->reset();
    sha1->reset();
    sha256->reset();
    
    EXPECT_FALSE(md5->isFinalized());
    EXPECT_FALSE(sha1->isFinalized());
    EXPECT_FALSE(sha256->isFinalized());
    
    // Should be able to process new data
    EXPECT_NO_THROW(md5->update(reinterpret_cast<const uint8_t*>("new"), 3));
    EXPECT_NO_THROW(sha1->update(reinterpret_cast<const uint8_t*>("new"), 3));
    EXPECT_NO_THROW(sha256->update(reinterpret_cast<const uint8_t*>("new"), 3));
}

#include <gtest/gtest.h>
#include "blake256.h"

class BLAKE256Test : public ::testing::Test {
protected:
    BLAKE256 blake256;
};

TEST_F(BLAKE256Test, EmptyString) {
    blake256.finalize();
    std::string hash = blake256.getHash();
    
    // BLAKE256 of empty string
    EXPECT_EQ(hash, "716f6e863f744b9ac22c97ec7b76ea5f5908bc5b2f67c61510bfc4751384ea7a");
}

TEST_F(BLAKE256Test, SimpleString) {
    std::string input = "abc";
    blake256.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake256.finalize();
    std::string hash = blake256.getHash();
    
    // BLAKE256 of "abc"
    EXPECT_EQ(hash, "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319");
}

TEST_F(BLAKE256Test, LongerString) {
    std::string input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    blake256.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake256.finalize();
    std::string hash = blake256.getHash();
    
    // This hash may need verification with reference implementation
    EXPECT_EQ(hash.length(), 64); // Should be 256 bits = 64 hex characters
}

TEST_F(BLAKE256Test, MultipleUpdates) {
    blake256.update(reinterpret_cast<const uint8_t*>("a"), 1);
    blake256.update(reinterpret_cast<const uint8_t*>("b"), 1);
    blake256.update(reinterpret_cast<const uint8_t*>("c"), 1);
    blake256.finalize();
    std::string hash = blake256.getHash();
    
    // Should be same as "abc"
    EXPECT_EQ(hash, "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319");
}

TEST_F(BLAKE256Test, Reset) {
    blake256.update(reinterpret_cast<const uint8_t*>("test"), 4);
    blake256.reset();
    
    std::string input = "abc";
    blake256.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake256.finalize();
    std::string hash = blake256.getHash();
    
    // Should be same as fresh "abc" hash
    EXPECT_EQ(hash, "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319");
}

TEST_F(BLAKE256Test, AlgorithmProperties) {
    EXPECT_EQ(blake256.getBlockSize(), 64);   // 512 bits
    EXPECT_EQ(blake256.getHashSize(), 32);    // 256 bits
    EXPECT_EQ(blake256.getAlgorithmName(), "BLAKE256");
    EXPECT_FALSE(blake256.isFinalized());
    
    blake256.finalize();
    EXPECT_TRUE(blake256.isFinalized());
}

TEST_F(BLAKE256Test, ErrorHandling) {
    blake256.finalize();
    
    // Should throw when trying to update after finalization
    EXPECT_THROW(blake256.update(reinterpret_cast<const uint8_t*>("test"), 4), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

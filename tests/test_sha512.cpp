#include <gtest/gtest.h>
#include "sha512.h"

class SHA512Test : public ::testing::Test {
protected:
    SHA512 sha512;
};

TEST_F(SHA512Test, EmptyString) {
    sha512.finalize();
    std::string hash = sha512.getHash();
    
    // SHA512 of empty string
    EXPECT_EQ(hash, "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

TEST_F(SHA512Test, SimpleString) {
    std::string input = "abc";
    sha512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    sha512.finalize();
    std::string hash = sha512.getHash();
    
    // SHA512 of "abc"
    EXPECT_EQ(hash, "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST_F(SHA512Test, LongerString) {
    std::string input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    sha512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    sha512.finalize();
    std::string hash = sha512.getHash();
    
    // SHA512 of the longer test string
    EXPECT_EQ(hash, "204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445");
}

TEST_F(SHA512Test, MultipleUpdates) {
    sha512.update(reinterpret_cast<const uint8_t*>("a"), 1);
    sha512.update(reinterpret_cast<const uint8_t*>("b"), 1);
    sha512.update(reinterpret_cast<const uint8_t*>("c"), 1);
    sha512.finalize();
    std::string hash = sha512.getHash();
    
    // Should be same as "abc"
    EXPECT_EQ(hash, "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST_F(SHA512Test, Reset) {
    sha512.update(reinterpret_cast<const uint8_t*>("test"), 4);
    sha512.reset();
    
    std::string input = "abc";
    sha512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    sha512.finalize();
    std::string hash = sha512.getHash();
    
    // Should be same as fresh "abc" hash
    EXPECT_EQ(hash, "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
}

TEST_F(SHA512Test, AlgorithmProperties) {
    EXPECT_EQ(sha512.getBlockSize(), 128);  // 1024 bits
    EXPECT_EQ(sha512.getHashSize(), 64);    // 512 bits
    EXPECT_EQ(sha512.getAlgorithmName(), "SHA512");
    EXPECT_FALSE(sha512.isFinalized());
    
    sha512.finalize();
    EXPECT_TRUE(sha512.isFinalized());
}

TEST_F(SHA512Test, ErrorHandling) {
    sha512.finalize();
    
    // Should throw when trying to update after finalization
    EXPECT_THROW(sha512.update(reinterpret_cast<const uint8_t*>("test"), 4), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

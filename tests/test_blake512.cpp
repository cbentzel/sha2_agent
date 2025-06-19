#include <gtest/gtest.h>
#include "blake512.h"

class BLAKE512Test : public ::testing::Test {
protected:
    BLAKE512 blake512;
};

TEST_F(BLAKE512Test, EmptyString) {
    blake512.finalize();
    std::string hash = blake512.getHash();
    
    // BLAKE512 of empty string
    EXPECT_EQ(hash, "a8cfbbd73726062df0c6864dda65defe58ef0cc52a5625090fa17601e1eecd1b628e94f396ae402a00acc9eab77b4d4c2e852aaaa25a636d80af3fc7913ef5b8");
}

TEST_F(BLAKE512Test, SimpleString) {
    std::string input = "abc";
    blake512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake512.finalize();
    std::string hash = blake512.getHash();
    
    // BLAKE512 of "abc"
    EXPECT_EQ(hash, "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923");
}

TEST_F(BLAKE512Test, LongerString) {
    std::string input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    blake512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake512.finalize();
    std::string hash = blake512.getHash();
    
    // This hash may need verification with reference implementation
    EXPECT_EQ(hash.length(), 128); // Should be 512 bits = 128 hex characters
}

TEST_F(BLAKE512Test, MultipleUpdates) {
    blake512.update(reinterpret_cast<const uint8_t*>("a"), 1);
    blake512.update(reinterpret_cast<const uint8_t*>("b"), 1);
    blake512.update(reinterpret_cast<const uint8_t*>("c"), 1);
    blake512.finalize();
    std::string hash = blake512.getHash();
    
    // Should be same as "abc"
    EXPECT_EQ(hash, "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923");
}

TEST_F(BLAKE512Test, Reset) {
    blake512.update(reinterpret_cast<const uint8_t*>("test"), 4);
    blake512.reset();
    
    std::string input = "abc";
    blake512.update(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
    blake512.finalize();
    std::string hash = blake512.getHash();
    
    // Should be same as fresh "abc" hash
    EXPECT_EQ(hash, "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923");
}

TEST_F(BLAKE512Test, AlgorithmProperties) {
    EXPECT_EQ(blake512.getBlockSize(), 128);  // 1024 bits
    EXPECT_EQ(blake512.getHashSize(), 64);    // 512 bits
    EXPECT_EQ(blake512.getAlgorithmName(), "BLAKE512");
    EXPECT_FALSE(blake512.isFinalized());
    
    blake512.finalize();
    EXPECT_TRUE(blake512.isFinalized());
}

TEST_F(BLAKE512Test, ErrorHandling) {
    blake512.finalize();
    
    // Should throw when trying to update after finalization
    EXPECT_THROW(blake512.update(reinterpret_cast<const uint8_t*>("test"), 4), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

cmake_minimum_required(VERSION 3.10)
project(hashgen VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(src)

# Main executable
add_executable(hashgen
    src/main.cpp
    src/stream_processor.cpp
    src/sha256.cpp
    src/sha1.cpp
    src/md5.cpp
    src/hash_factory.cpp
)

# Create a library for testing
add_library(hash_lib STATIC
    src/stream_processor.cpp
    src/sha256.cpp
    src/sha1.cpp
    src/md5.cpp
    src/hash_factory.cpp
)

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)

if(GTest_FOUND)
    # Test executable (all tests)
    add_executable(hash_tests
        tests/test_hash.cpp
        tests/test_md5.cpp
        tests/test_sha1.cpp
    )
    
    target_link_libraries(hash_tests
        hash_lib
        GTest::GTest
        GTest::Main
    )
    
    # Individual test executables
    add_executable(md5_tests
        tests/test_md5.cpp
    )
    
    target_link_libraries(md5_tests
        hash_lib
        GTest::GTest
        GTest::Main
    )
    
    add_executable(sha1_tests
        tests/test_sha1.cpp
    )
    
    target_link_libraries(sha1_tests
        hash_lib
        GTest::GTest
        GTest::Main
    )
    
    # Keep the existing SHA256 tests
    add_executable(sha256_tests
        tests/test_sha256.cpp
    )
    
    target_link_libraries(sha256_tests
        hash_lib
        GTest::GTest
        GTest::Main
    )
    
    # Add tests to CTest
    add_test(NAME HashTests COMMAND hash_tests)
    add_test(NAME MD5Tests COMMAND md5_tests)
    add_test(NAME SHA1Tests COMMAND sha1_tests)
    add_test(NAME SHA256Tests COMMAND sha256_tests)
    
    # Enable verbose test output
    set_property(TEST HashTests PROPERTY ENVIRONMENT "GTEST_OUTPUT=xml")
else()
    message(STATUS "Google Test not found. Tests will not be built.")
    message(STATUS "To install Google Test on macOS: brew install googletest")
endif()

# Install targets
install(TARGETS hashgen DESTINATION bin)
install(FILES docs/hashgen.1 DESTINATION share/man/man1)

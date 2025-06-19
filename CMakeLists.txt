cmake_minimum_required(VERSION 3.10)
project(sha256_hasher VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(src)

# Main executable
add_executable(sha256sum
    src/main.cpp
    src/sha256.cpp
)

# Create a library for testing
add_library(sha256_lib STATIC
    src/sha256.cpp
)

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)

if(GTest_FOUND)
    # Test executable
    add_executable(sha256_tests
        tests/test_sha256.cpp
    )
    
    target_link_libraries(sha256_tests
        sha256_lib
        GTest::GTest
        GTest::Main
    )
    
    # Add test to CTest
    add_test(NAME SHA256Tests COMMAND sha256_tests)
    
    # Enable verbose test output
    set_property(TEST SHA256Tests PROPERTY ENVIRONMENT "GTEST_OUTPUT=xml")
else()
    message(STATUS "Google Test not found. Tests will not be built.")
    message(STATUS "To install Google Test on macOS: brew install googletest")
endif()

# Install targets
install(TARGETS sha256sum DESTINATION bin)
install(FILES docs/sha256sum.1 DESTINATION share/man/man1)

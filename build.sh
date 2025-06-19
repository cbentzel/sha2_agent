#!/bin/bash

# Build script for SHA256 Hash Calculator
set -e

echo "Building SHA256 Hash Calculator..."

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building..."
make

echo "Build complete!"
echo ""
echo "Executable: ./sha256sum"
echo ""
echo "To run tests (requires Google Test):"
echo "  make test"
echo ""
echo "To install system-wide:"
echo "  sudo make install"
echo ""
echo "Example usage:"
echo "  echo -n 'hello world' | ./sha256sum"

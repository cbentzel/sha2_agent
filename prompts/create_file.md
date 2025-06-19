The goal is to create an application which calculates a sha256 hash on an input file, which is passed in stdout. So this should be a command line utility which can run on Mac.

Here are the general rules:

- Go all the way to full completion of generating code.
- Use C++ as the programming language. You can use C++14 or earlier.
- Prefer using CMake as the build file approach
- The data should be streamed in - only use max of something like 32K buffer of RAM internally
- Input should come in stdin, output writes to stdout the sha256 hash
- Only standard library files can be used
- The output should also include documentation in the form of a README file and a man page file
- Unit tests with gtest framework should also be in place
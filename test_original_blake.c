#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test what the expected vectors should be for empty string and "abc"
// Based on what I found in the GitHub repo

int main() {
    printf("Testing expected BLAKE vectors:\n");
    
    // BLAKE256 test vectors from reference implementation:
    // test1 (1 byte of 0): 0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87
    printf("BLAKE256 empty (1 byte 0): 0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87\n");
    printf("Our test expects:          716f6e863f744b9ac22c97ec7b76ea5f5908bc5b2f67c61510bfc4751384ea7a\n");
    printf("Our impl produces:         a3f79e134fa731660ac3cce24e7d752034128dfe97ad90cedda24ff329704475\n");
    printf("\n");
    
    // For "abc" we need to find what the reference produces
    printf("BLAKE256 'abc':\n");
    printf("Our test expects:          bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319\n");
    printf("Our impl produces:         cc0a6fc0f8d9b7816f8341b01c8d330a766d62fd3f486b7816d7068642d3d2b7\n");
    printf("\n");
    
    return 0;
}

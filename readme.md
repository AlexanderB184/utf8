# UTF8

A simple header-only library for parsing utf8 in C/C++.

## Features

- **No Standard Library**
    This library doesn't link to any functions in the C stdlib to ensure maximum portability.
- **No Allocations**
    This library never allocates data for you, all functions take in user allocated buffers, returning error values if there isn't enough space. This gives greater control to the user as to how they want to allocate their data and they never have to worry about freeing data from this library.
- **C strings**
    All functions have versions for both null terminated strings and strings with length. Allowing the greater flexability for many different use cases. Use the `_nt` suffix to use the null terminated version.
- **Error Handling**
    Any invalid utf8 character decodes as the unicode replacement character U+FFFD `�`. Invalid encodings are considered to have a length of one to prevent malformed characters from "hiding" valid characters.

## Core Functions

- **Validation**
    Functions to check if a string is valid utf8 (or is 7bit ascii)
- **Counting UTF8**
    Functions to count the number of unicode characters in a utf8 string
- **Encoding and Decoding**
    Functions to convert between utf8 and utf32

## Example

``` C
// define `UNICODE_IMPL` before including `unicode.h` in exactly one file.
#define UNICODE_IMPL
#include "unicode.h"

int find_char(utf8_t* text, utf32_t codepoint) {

    int idx = 0;

    // loop until null terminator
    while (text[idx]) {
        
        // decode char
        decoded_utf8_t decoded = utf8_decode_nt(text + idx);

        // compare with target char
        if (decoded.codepoint == codepoint) {
            return idx;
        }

        // iterate by length of utf8 char
        idx += decoded.len;

    }
}
```
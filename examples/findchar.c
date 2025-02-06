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
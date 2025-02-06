#define UNICODE_IMPL
#include "../unicode.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

void fuzz(void)
{
    uint64_t count = 0;
    for (uint64_t x = 0; x <= UINT32_MAX; x++) {
        char buffer[4];
        memcpy(buffer, &x, 4);
        decoded_utf8_t decoded = utf8_decode((utf8_t*)buffer, 4);
        if (decoded.codepoint == UNICODE_REPLACEMENT_CHAR) {
            if (utf8_is_valid_utf8((utf8_t*)buffer, 4) && memcmp("�", buffer, 3) != 0) {
                printf("PROBLEM CHAR INVALID WHEN IT SHOULDN'T BE!\n");
                break;
            }
           continue;
        }
        
        count++;
        char buffer2[4];
        uint32_t len = utf8_encode((utf8_t*)buffer2, 4, decoded.codepoint);
        if (memcmp(buffer2, buffer, len) != 0) {
            printf("PROBLEM ENCODED AND PREVIOUS DO NOT MATCH!\n");
            break;
        }
        
        if (x % 0x01000000 == 0) {
            printf("x: %llx\n", x);
        }
    }
    printf("total valid found: %llu\n", count);
}

void fuzz_valid(void)
{
    uint64_t count = 0;
    for (uint64_t x = 0; x <= UINT32_MAX; x++) {
        char buffer[4];
        memcpy(buffer, &x, 4);
        buffer[1] = 0b10000000 | (0b00111111 & buffer[1]);
        buffer[2] = 0b10000000 | (0b00111111 & buffer[2]);
        buffer[3] = 0b10000000 | (0b00111111 & buffer[3]);
        decoded_utf8_t decoded = utf8_decode((utf8_t*)buffer, 4);
        if (decoded.codepoint == UNICODE_REPLACEMENT_CHAR) {
            /*if (memcmp("�", buffer, 3) != 0) {
                printf("PROBLEM CHAR INVALID WHEN IT SHOULDN'T BE!\n");
                break;
            }*/
           continue;
        }
        count++;
        char buffer2[4];
        uint32_t len = utf8_encode((utf8_t*)buffer2, 4, decoded.codepoint);
        if (memcmp(buffer2, buffer, len) != 0) {
            printf("PROBLEM ENCODED AND PREVIOUS DO NOT MATCH!\n");
            break;
        }
        
        if (x % 0x01000000 == 0) {
            printf("x: %llx\n", x);
        }
    }
    printf("total valid found: %llu\n", count);
}

int main(void)
{
    utf8_t* utf8 = UTF8_CAST("abc값de%%ႢfghiႦjkl𐰋mnoႠpqrႳ가stuvႧ갌wxyz😂🤨🧐🥸🙂🥳📅ñ÷ùþ©®«¥¡#$^&*()𐰏𐰖");
    uint32_t i = 0;
    utf32_t codepoint = 0;
    do  {
        decoded_utf8_t decoded = utf8_decode_nt(utf8 + i);
        codepoint = decoded.codepoint;
        utf8_t buffer[4];
        uint32_t len = utf8_encode(buffer, 4, codepoint);
        assert(len == decoded.len);
        if (memcmp(utf8 + i, buffer, len) != 0) {
            printf("cp: %04x\nlen: %u\nnewlen: %u\nold: %s\nmine: %.*s\n", codepoint, decoded.len, len, (char*)(utf8 + i), len, (char*)buffer);
            assert(0); 
        }
        i += decoded.len;
    } while (codepoint);

    fuzz();
}


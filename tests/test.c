#define UNICODE_IMPL
#include "../unicode.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    assert(argc >= 1);
    for (int i = 0; i < argc; i++) {
        uint32_t c = utf8_count_nt(argv[i]);
        printf("str: \"%s\"\ncount: %u\n", argv[i], c);
    }
    return 0;
}
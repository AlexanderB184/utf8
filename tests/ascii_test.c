#define UNICODE_IMPL
#include "../unicode.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void)
{
    const utf8_t* utf8 = UTF8_CAST("abcdefghijklmnopqrstuvwxyz 123124567890\n");
    assert(utf8_is_7bit_ascii_string_nt(utf8));
    utf8 = UTF8_CAST("");
    assert(utf8_is_7bit_ascii_string_nt(utf8));
    utf8 = UTF8_CAST("hello world!");
    assert(utf8_is_7bit_ascii_string_nt(utf8));
    utf8 = UTF8_CAST("abc값de%%ႢfghiႦjkl𐰋mnoႠpqrႳ가stuvႧ갌wxyz😂🤨🧐🥸🙂🥳📅ñ÷ùþ©®«¥¡#$^&*()𐰏𐰖");
    assert(!utf8_is_7bit_ascii_string_nt(utf8));
    utf8 = UTF8_CAST("abcdefghijklmnopqrstuvwxyz 123124567890\n");
    assert(utf8_is_7bit_ascii_string(utf8, sizeof("abcdefghijklmnopqrstuvwxyz 123124567890\n")));
    utf8 = UTF8_CAST("");
    assert(utf8_is_7bit_ascii_string(utf8, sizeof("")));
    utf8 = UTF8_CAST("hello world!");
    assert(utf8_is_7bit_ascii_string(utf8, sizeof("hello world!")));
    utf8 = UTF8_CAST("abc값de%%ႢfghiႦjkl𐰋mnoႠpqrႳ가stuvႧ갌wxyz😂🤨🧐🥸🙂🥳📅ñ÷ùþ©®«¥¡#$^&*()𐰏𐰖");
    assert(!utf8_is_7bit_ascii_string(utf8, sizeof("abc값de%%ႢfghiႦjkl𐰋mnoႠpqrႳ가stuvႧ갌wxyz😂🤨🧐🥸🙂🥳📅ñ÷ùþ©®«¥¡#$^&*()𐰏𐰖")));
}


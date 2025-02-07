#ifndef UNICODE_H
#define UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// a unicode codepoint
typedef uint32_t utf32_t;

// a single byte in a utf8 encoded string
typedef uint8_t utf8_t;

// casts char* pointer to utf8_t pointer
#define UTF8_CAST(str) (const utf8_t*)(str)

// utf8 encoded c literal string
#define UTF8_LITERAL(cliteral) (const utf8_t*)(u8##cliteral)

// output of `utf8_codepoint_length` if the codepoint is invalid, i.e. `codepoint > 0x10FFFF`.
#define UNICODE_INVALID_CODEPOINT UINT32_MAX

// codepoint of unicode replacement char. Returned when `utf8_decode` encounters an invalid utf8 encoding.
#define UNICODE_REPLACEMENT_CHAR (utf32_t)(0xFFFDU)

// utf8 encoding of unicode replacement char.
#define UNICODE_REPLACEMENT_CHAR_UFT8 UTF8_LITERAL("�")

// returned from `utf8_decode`, 
// includes the decoded codepoint `codepoint` and the length `len`. 
// If an error is encounted the unicode replacement character "�" is returned as the codepoint and the length is 1.
typedef struct decoded_utf8_t {
  utf32_t codepoint; // decoded codepoint, if an error is encounter it will be the unicode replacement character U+FFFD "�".
  uint32_t len;      // number of bytes decoded, 1 if an error is encountered.
} decoded_utf8_t;

#ifdef __cplusplus
#define DECODED_UTF8_LITERAL(CODEPOINT, LENGTH) (decoded_utf8_t{(CODEPOINT), (LENGTH)})
#else
#define DECODED_UTF8_LITERAL(CODEPOINT, LENGTH) ((decoded_utf8_t){(CODEPOINT), (LENGTH)})
#endif

/// @brief checks if the byte is a valid continuation byte, i.e. 0b10xxxxxx
/// @param byte the continuation byte to check.
/// @return `true` if it is a valid continuation, `false` if not.
bool utf8_is_continuation(uint8_t byte);

/// @brief checks if the byte is a valid header byte for a utf8 encoded character, 
/// i.e. it is 7bit ascii `0b0xxxxxxx`, 
/// 2 byte utf8 header `0b110xxxxx`, 
/// 3 byte utf8 header `0b1110xxxx`, 
/// or a 4 byte utf8 header `0b11110xxx`.
/// @param head the head byte of the utf8 encoded character
/// @return `true` if it is a valid continuation, `false` if not.
bool utf8_is_valid_head(uint8_t head);

/// @brief checks if a single utf8 encoded character is completely valid, 
/// checking it has a valid head, isn't truncated, isn't overlong, 
/// and isn't a codepoint greater than U+10FFFF.
/// @param utf8 the pointer to the first byte of the utf8 encoded character
/// @param len  length of the buffer the character is stored in
/// @return     `true` if the character is a valid utf8 encoding, `false` if not.
bool utf8_is_valid(const utf8_t* utf8, uint32_t len);

/// @brief null terminated version of `utf8_is_valid`. 
/// checks if a single utf8 encoded character is completely valid, 
/// checking it has a valid head, isn't truncated, isn't overlong, 
/// and isn't a codepoint greater than U+10FFFF.
/// @param utf8 pointer to the first byte of the utf8 encoded character
/// @return     `true` if the character is a valid utf8 encoding, `false` if not.
bool utf8_is_valid_nt(const utf8_t* utf8);

bool utf8_is_valid_string(const utf8_t* utf8, uint32_t len);
bool utf8_is_valid_string_nt(const utf8_t* utf8);

/// @brief checks utf8 character us not overlong, 
/// an overlong utf8 character is a multi-byte character with a codepoint that 
/// could've fit in a smaller multi-byte character. 
/// i.e. the sequence `"C0 E0"` decodes as U+60 which can fit in a single byte. 
/// Assumes the character is otherwise valid, may read past the byte at `utf8` so it 
/// should only be used after validating that the character is not truncated.
/// @param utf8 pointer to the first byte of the utf8 encoded character
/// @return `true` if the character is an overlong encoding, `false` if not.
bool utf8_is_overlong_encoding(const utf8_t* utf8);

/// @brief checks if a codepoint is valid, i.e. no greater than U+10FFFF.
/// Assumes the character is otherwise valid, may read past the byte at `utf8` so it 
/// should only be used after validating that the character is not truncated.
/// @param codepoint 
bool utf8_is_valid_codepoint(utf32_t codepoint);

/// @brief checks if a codepoint is valid, i.e. no greater than U+10FFFF.
/// @param codepoint 
bool utf8_is_oversize_codepoint(const utf8_t* utf8);

/// @brief gets the length of the utf8 encoded character. Returns 1 if it is invalid.
/// @param utf8 pointer to the first byte in the utf8 encoded character. 
/// @return the length of the character in bytes
uint32_t utf8_length(const utf8_t* utf8);

/// @brief gets the length of the utf8 encoding of the codepoint. returns `UNICODE_INVALID_CODEPOINT` if the codepoint is invalid.
/// @param codepoint 
/// @return the length of the character in bytes
uint32_t utf8_codepoint_length(utf32_t codepoint);

/// @brief counts the number of seperate utf8 characters in a string, including errors.
/// @param str the utf8 encoded string
/// @param len the length of the string in bytes
/// @return the number of sperate utf8 characters, including errors.
uint32_t utf8_count(const utf8_t* str, size_t len);

/// @brief null terminated version of `utf8_count`.
/// counts the number of seperate utf8 characters in a string, including errors.
/// @param str the utf8 encoded string
/// @return the number of sperate utf8 characters, including errors and excluding the null terminator.
uint32_t utf8_count_nt(const utf8_t* str);

/// @brief decodes a single character from the string. 
/// If an invalid utf8 encoding is encounter it returns the replacement character U+FFFD "�" and a length of 1.
/// @param str the utf8 encoded string
/// @param len the length of the string in bytes
/// @return decoded_utf8, contains both the decoded codepoint and the number of bytes read. 
///
/// struct decoded_utf8_t {
///
///   utf32_t codepoint;\n
///
///   uint32_t len;
///
/// };
decoded_utf8_t utf8_decode(const utf8_t* str, size_t len);

/// @brief null terminated version of `utf8_decode`.
/// decodes a single character from the string. 
/// If an invalid utf8 encoding is encounter it returns the replacement character U+FFFD "�" and a length of 1.
/// When the null terminator is encounter it returns the null character U+0 and a length of 1.
/// @param str the utf8 encoded string
/// @param len the length of the string in bytes
/// @return decoded_utf8, contains both the decoded codepoint and the number of bytes read. 
///
/// struct decoded_utf8_t {
///
///   utf32_t codepoint;\n
///
///   uint32_t len;
///
/// };
decoded_utf8_t utf8_decode_nt(const utf8_t* str);

/// @brief encodes a single unicode character in utf8, storing it in buffer and returning the number of bytes written.
/// If there is insufficient space in the buffer to store the characer, it doesn't write the character and returns 0.
/// If the codepoint is invalid, i.e. > U+10FFFF it returns `UNICODE_INVALID_CODEPOINT`
/// @param buffer the buffer to write to. must not be null.
/// @param len the length of the buffer.
/// @param codepoint the character to write
/// @return the number of bytes written, or `UNICODE_INVALID_CODEPOINT` if the codepoint is invalid.
size_t utf8_encode(utf8_t* buffer, size_t len, utf32_t codepoint);

/// @brief null terminated version of `utf8_encode`, puts a null character after the encoded character so that the buffer is always a valid null terminated string.
/// factors space for null terminator in check for space in buffer.
/// encodes a single unicode character in utf8, storing it in buffer and returning the number of bytes written.
/// If there is insufficient space in the buffer to store the characer, it doesn't write the character and returns 0.
/// If the codepoint is invalid, i.e. > U+10FFFF it returns `UNICODE_INVALID_CODEPOINT`
/// @param buffer the buffer to write to. must not be null.
/// @param len the length of the buffer.
/// @param codepoint the character to write
/// @return the number of bytes written, or `UNICODE_INVALID_CODEPOINT` if the codepoint is invalid.
size_t utf8_encode_nt(utf8_t* buffer, size_t len, utf32_t codepoint);

/// @brief goes from continuation byte and iterates backwards until it finds the head byte of character. 
/// @warning Assumes valid utf8.
/// @param str pointer to arbitrary point in string
/// @return pointer to first byte of utf8 encoded character
utf8_t* utf8_goto_head(char* str);

/// @brief checks if byte is 7bit ascii
/// @param byte 
bool utf8_is_7bit_ascii(utf8_t byte);

/// @brief checks if string is 7bit ascii
bool utf8_is_7bit_ascii_string(const utf8_t* str, uint32_t len);

/// @brief null terminated version of `utf8_is_7bit_ascii_string_nt`
/// checks if string is 7bit ascii
bool utf8_is_7bit_ascii_string_nt(const utf8_t* str);

/* FUTURE `string.h` functionality to be added
int utf8_replace_malformed_tokens(utf8_t* str, uint32_t len, utf8_t chr);
int utf8_replace_malformed_tokens_nt(utf8_t* str, utf8_t chr);


int utf8_cmp(const utf8_t* lhs, uint32_t lhs_len, const utf8_t* rhs, uint32_t rhs_len);
int utf8_cmp_nt(const utf8_t* lhs, const utf8_t* rhs);

int utf8_find_char(const utf8_t* str, uint32_t len, const utf8_t* chr);
int utf8_find_char_nt(const utf8_t* str, const utf8_t* chr);

int utf8_find_substring(const utf8_t* str, uint32_t len, const utf8_t* substr, uint32_t sublen);
int utf8_find_substring_nt(const utf8_t* str, const utf8_t* substr);
*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif  // UNICODE_H

#ifdef UNICODE_IMPL

utf8_t* utf8_goto_head(char* str) {
    while (utf8_is_continuation(*str)) str--;
    return (utf8_t*)str;
}

bool utf8_is_7bit_ascii(utf8_t byte) {
    return byte < 0x80;
}

bool utf8_is_7bit_ascii_string(const utf8_t* str, uint32_t len) {
    
    for (uint32_t i = 0; i < len; i++) {
        if (!utf8_is_7bit_ascii(str[i])) {
            return false;
        }
    }
    return true;
}

bool utf8_is_7bit_ascii_string_nt(const utf8_t* str) {
    while (*str) {
        if (!utf8_is_7bit_ascii(*str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool utf8_is_valid_codepoint(utf32_t codepoint) {
    return codepoint <= 0x10FFFF;
}

bool utf8_is_continuation(uint8_t byte) {
    return (byte & 0xC0) == 0x80;
}

bool utf8_is_valid_head(uint8_t byte) {
    return !(byte > 0xF8) & !utf8_is_continuation(byte);
}

bool utf8_is_valid(const utf8_t* utf8, uint32_t len) {
    
    if (!utf8_is_valid_head(*utf8)) {
        return false;
    }

    uint32_t utf8_len = utf8_length(utf8);

    if (utf8_len > len) {
        return false;
    }
    
    for (uint32_t i = 1; i < utf8_len; i++) {
        if (!utf8_is_continuation(utf8[i])) {
            return false;
        }
    }

    if (utf8_is_overlong_encoding(utf8) || utf8_is_oversize_codepoint(utf8)) {
        return false;
    }

    return true;
}

bool utf8_is_valid_nt(const utf8_t* utf8) {
    
    if (!utf8_is_valid_head(*utf8)) {
        return false;
    }

    uint32_t utf8_len = utf8_length(utf8);

    for (uint32_t i = 1; i < utf8_len; i++) {
        if (!utf8[i] || !utf8_is_continuation(utf8[i])) {
            return false;
        }
    }

    if (utf8_is_overlong_encoding(utf8) || utf8_is_oversize_codepoint(utf8)) {
        return false;
    }

    return true;
}

bool utf8_is_valid_string(const utf8_t* utf8, uint32_t len) {
    do {
        if (!utf8_is_valid_head(*utf8)) {
            return false;
        }

        uint32_t utf8_len = utf8_length(utf8);

        if (utf8_len > len) {
            return false;
        }

        for (uint32_t i = 1; i < utf8_len; i++) {
            if (!utf8_is_continuation(utf8[i])) {
                return false;
            }
        }

        if (utf8_is_overlong_encoding(utf8) || utf8_is_oversize_codepoint(utf8)) {
            return false;
        }

        utf8 += utf8_len;
        len  -= utf8_len;
    } while (len > 0);
    return true;
}


bool utf8_is_valid_string_nt(const utf8_t* utf8) {
    while (*utf8) {
        if (!utf8_is_valid_head(*utf8)) {
            return false;
        }

        uint32_t utf8_len = utf8_length(utf8);

        for (uint32_t i = 1; i < utf8_len; i++) {
            if (!utf8[i] || !utf8_is_continuation(utf8[i])) {
                return false;
            }
        }

        if (utf8_is_overlong_encoding(utf8) || utf8_is_oversize_codepoint(utf8)) {
            return false;
        }

        utf8 += utf8_len;
    }
    return true;
}

uint32_t utf8_length(const utf8_t* utf8) {
    uint8_t head = *utf8;
    if ((head & 0x80) == 0x00) return 1;
    if ((head & 0xe0) == 0xc0) return 2;
    if ((head & 0xf0) == 0xe0) return 3;
    if ((head & 0xf8) == 0xf0) return 4;
    return 1;
}

uint32_t utf8_count(const utf8_t* str, size_t len) {
    uint32_t i = 0, c = 0;
    while (i < len) {
        if (utf8_is_valid(&str[i], len - i)) {
            i += utf8_length(&str[i]);
        } else {
            i += 1;
        }
        c ++;
    }
    return c;
}

uint32_t utf8_count_nt(const utf8_t* str) {
    uint32_t i = 0, c = 0;
    while (str[i]) {
        if (utf8_is_valid_nt(&str[i])) {
            i += utf8_length(&str[i]);
        } else {
            i += 1;
        }
        c ++;
    }
    return c;
}

uint32_t utf8_codepoint_length(utf32_t codepoint) {
    if (codepoint < 0x80) {
        return 1;
    }
    if (codepoint < 0x800) {
        return 2;
    }
    if (codepoint < 0x10000) {
        return 3;
    }
    if (codepoint < 0x110000) {
        return 4;
    }
    return UNICODE_INVALID_CODEPOINT;
}


bool utf8_is_overlong_encoding(const utf8_t* utf8) {
    uint32_t utf8_len = utf8_length(utf8);
    if (utf8_len == 1) return false;
    if (utf8_len == 2) return !(utf8[0] & 0x1E);
    if (utf8_len == 3) return !(utf8[0] & 0x0F) && !(utf8[1] & 0x40);
    if (utf8_len == 4) return !(utf8[0] & 0x07) && !(utf8[1] & 0x50);
    return true;
}

bool utf8_is_oversize_codepoint(const utf8_t* utf8) {
    uint32_t utf8_len = utf8_length(utf8);
    if (utf8_len < 4) return false;
    if (utf8_len == 4) return (utf8[0] & 0x04) && ((utf8[0] & 0x03) || (utf8[1] & 0x30));
    return true;
}

decoded_utf8_t utf8_decode(const utf8_t* str, size_t len) {
    uint32_t utf8_len = utf8_length(str);

    if (!utf8_is_valid(str, len)) {
        return DECODED_UTF8_LITERAL(UNICODE_REPLACEMENT_CHAR, 1);
    }

    if (utf8_len == 1) {
        return DECODED_UTF8_LITERAL(str[0], utf8_len);
    }

    utf32_t head_bit_mask = (0x7F >> utf8_len);

    utf32_t codepoint = str[0] & head_bit_mask;

    for (uint32_t i = 1; i < utf8_len; i++) {
        codepoint = (codepoint << 6) | (str[i] & 0x3F);
    }

    return DECODED_UTF8_LITERAL(codepoint, utf8_len);
}

decoded_utf8_t utf8_decode_nt(const utf8_t* str) {
    if (!*str) return DECODED_UTF8_LITERAL(0, 1);

    uint32_t utf8_len = utf8_length(str);

    if (!utf8_is_valid_nt(str)) {
        return DECODED_UTF8_LITERAL(UNICODE_REPLACEMENT_CHAR, 1);
    }

    if (utf8_len == 1) {
        return DECODED_UTF8_LITERAL(str[0], utf8_len);
    }

    utf32_t head_bit_mask = (0x7F >> utf8_len);

    utf32_t codepoint = str[0] & head_bit_mask;

    for (uint32_t i = 1; i < utf8_len; i++) {
        codepoint = (codepoint << 6) | (str[i] & 0x3F);
    }

    return DECODED_UTF8_LITERAL(codepoint, utf8_len);
}

size_t utf8_encode(utf8_t* buffer, size_t len, utf32_t codepoint) {


    if (!utf8_is_valid_codepoint(codepoint)) {
        return UNICODE_INVALID_CODEPOINT;
    }

    uint32_t utf8_len = utf8_codepoint_length(codepoint);

    if (utf8_len > len) {
        return 0;
    }

    if (utf8_len == 1) {
        buffer[0] = codepoint;
        return 1;
    }

    for (int32_t i = utf8_len - 1; i > 0; i--) {
        buffer[i] = (codepoint & 0x3F) | 0x80;
        codepoint = codepoint >> 6;
    }

    uint8_t head_bits = (0xF0 << (4 - utf8_len));

    buffer[0] = head_bits | codepoint;
    

    return utf8_len;
}

size_t utf8_encode_nt(utf8_t* buffer, size_t len, utf32_t codepoint) {


    if (!utf8_is_valid_codepoint(codepoint)) {
        return UNICODE_INVALID_CODEPOINT;
    }

    uint32_t utf8_len = utf8_codepoint_length(codepoint);

    // dont forget the space for the null terminator!!!
    if (utf8_len + 1 > len) {
        if (len >= 1) {
            buffer[0] = 0;
        }
        return 0;
    }

    buffer[utf8_len] = 0;

    if (utf8_len == 1) {
        buffer[0] = codepoint;
        return 1;
    }

    for (int32_t i = utf8_len - 1; i > 0; i--) {
        buffer[i] = (codepoint & 0x3F) | 0x80;
        codepoint = codepoint >> 6;
    }

    uint8_t head_bits = (0xF0 << (4 - utf8_len));

    buffer[0] = head_bits | codepoint;

    return utf8_len;
}

#endif  // UNICODE_IMPL
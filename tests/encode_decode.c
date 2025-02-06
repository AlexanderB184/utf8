#define UNICODE_IMPL
#include "../unicode.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define NTRIALS 100
#define WARMUP 3

int main(void)
{
    for (int i = 0; i < WARMUP; i++){
        for (utf32_t cp = 0; cp <= 0x10FFFF; cp++) {
            utf8_t buffer[4];
            uint32_t len = utf8_encode(buffer, 4, cp);
            assert(utf8_codepoint_length(cp) == len);
            decoded_utf8_t decoded = utf8_decode(buffer, 4);
            if (decoded.len != len || decoded.codepoint != cp) {
                printf("cp: %04x\ndecoded cp: %04x\nlen: %u\n decoded len: %u\n", cp, decoded.codepoint, len, decoded.len);
                break;
            }
        }
    }
    size_t trials[NTRIALS];
    for (int i = 0; i < NTRIALS; i++){
        struct timespec start;
        clock_gettime(0, &start);
        for (utf32_t cp = 0; cp <= 0x10FFFF; cp++) {
            utf8_t buffer[4];
            uint32_t len = utf8_encode(buffer, 4, cp);
            assert(utf8_codepoint_length(cp) == len);
            decoded_utf8_t decoded = utf8_decode(buffer, 4);
            if (decoded.len != len || decoded.codepoint != cp) {
                printf("cp: %04x\ndecoded cp: %04x\nlen: %u\n decoded len: %u\n", cp, decoded.codepoint, len, decoded.len);
                break;
            }
        }
        struct timespec end;
        clock_gettime(0, &end);

        long nanos = (end.tv_nsec - start.tv_nsec) + (end.tv_sec - start.tv_sec) * (1000 * 1000 * 1000);

        trials[i] = nanos;
        struct timespec req = {.tv_sec=0, .tv_nsec = i * 10 * 1000};
        nanosleep(&req, NULL);
    }
    double avg = 0;
    for (int i = 0; i < NTRIALS; i++){
        avg += (double)trials[i] / NTRIALS;
    }
    double var = 0;
    for (int i = 0; i < NTRIALS; i++){
        var += (trials[i] - avg) * (trials[i] - avg) / NTRIALS;
    }
    var = sqrt(var);
    printf("trials:   %d\n", NTRIALS);
    printf("avg time: %.2f\n", avg);
    printf("std dev:  %.2f\n", var);
}
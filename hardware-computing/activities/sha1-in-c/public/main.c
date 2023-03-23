#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

#define SHA1_LEN 20

/* Code based on the official Intel implementation, from here:
 * https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sha-extensions.html
 */

void sha1(const uint8_t data[], uint32_t length, uint32_t out[5])
{
    __m128i ABCD, ABCD_SAVE, E0, E0_SAVE, E1;
    __m128i MSG0, MSG1, MSG2, MSG3;

    /* 
     * input bytes are in a different order than SHA-NI wants them;
     * shuffle the bytes
     */
    const __m128i MASK = _mm_set_epi64x(0x0001020304050607ULL, 0x08090a0b0c0d0e0fULL);

    /* initial SHA-1 state, defined as such by the algorithm */
    uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

    /* Load initial values */
    ABCD = _mm_loadu_si128((const __m128i*) state);
    E0 = _mm_set_epi32(state[4], 0, 0, 0);
    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);

    while (length >= 64)
    {
        /* Save current state  */
        ABCD_SAVE = ABCD;
        E0_SAVE = E0;

        /* Rounds 0-3 */
        MSG0 = _mm_loadu_si128((const __m128i*)(data + 0));
        MSG0 = _mm_shuffle_epi8(MSG0, MASK);
        E0 = _mm_add_epi32(E0, MSG0);
        E1 = ABCD;
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);

        /* Rounds 4-7 */
        MSG1 = _mm_loadu_si128((const __m128i*)(data + 16));
        MSG1 = _mm_shuffle_epi8(MSG1, MASK);
        E1 = _mm_sha1nexte_epu32(E1, MSG1);
        E0 = ABCD;
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
        MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);

        /* Rounds 8-11 */
        MSG2 = _mm_loadu_si128((const __m128i*)(data + 32));
        MSG2 = _mm_shuffle_epi8(MSG2, MASK);
        E0 = _mm_sha1nexte_epu32(E0, MSG2);
        E1 = ABCD;
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
        MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
        MSG0 = _mm_xor_si128(MSG0, MSG2);

        /* Rounds 12-15 */
        MSG3 = _mm_loadu_si128((const __m128i*)(data + 48));
        MSG3 = _mm_shuffle_epi8(MSG3, MASK);
        E1 = _mm_sha1nexte_epu32(E1, MSG3);
        E0 = ABCD;
        MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
        MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
        MSG1 = _mm_xor_si128(MSG1, MSG3);

        /* Rounds 16-19 */
        E0 = _mm_sha1nexte_epu32(E0, MSG0);
        E1 = ABCD;
        MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
        MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
        MSG2 = _mm_xor_si128(MSG2, MSG0);

        /* Rounds 20-23 */
        E1 = _mm_sha1nexte_epu32(E1, MSG1);
        E0 = ABCD;
        MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
        MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
        MSG3 = _mm_xor_si128(MSG3, MSG1);

        /* Rounds 24-27 */
        E0 = _mm_sha1nexte_epu32(E0, MSG2);
        E1 = ABCD;
        MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
        MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
        MSG0 = _mm_xor_si128(MSG0, MSG2);

        /* Rounds 28-31 */
        E1 = _mm_sha1nexte_epu32(E1, MSG3);
        E0 = ABCD;
        MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
        MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
        MSG1 = _mm_xor_si128(MSG1, MSG3);

        /* Rounds 32-35 */
        E0 = _mm_sha1nexte_epu32(E0, MSG0);
        E1 = ABCD;
        MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
        MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
        MSG2 = _mm_xor_si128(MSG2, MSG0);

        /* Rounds 36-39 */
        E1 = _mm_sha1nexte_epu32(E1, MSG1);
        E0 = ABCD;
        MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
        MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
        MSG3 = _mm_xor_si128(MSG3, MSG1);

        /* Rounds 40-43 */
        E0 = _mm_sha1nexte_epu32(E0, MSG2);
        E1 = ABCD;
        MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
        MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
        MSG0 = _mm_xor_si128(MSG0, MSG2);

        /* Rounds 44-47 */
        E1 = _mm_sha1nexte_epu32(E1, MSG3);
        E0 = ABCD;
        MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
        MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
        MSG1 = _mm_xor_si128(MSG1, MSG3);

        /* Rounds 48-51 */
        E0 = _mm_sha1nexte_epu32(E0, MSG0);
        E1 = ABCD;
        MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
        MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
        MSG2 = _mm_xor_si128(MSG2, MSG0);

        /* Rounds 52-55 */
        E1 = _mm_sha1nexte_epu32(E1, MSG1);
        E0 = ABCD;
        MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
        MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
        MSG3 = _mm_xor_si128(MSG3, MSG1);

        /* Rounds 56-59 */
        E0 = _mm_sha1nexte_epu32(E0, MSG2);
        E1 = ABCD;
        MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
        MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
        MSG0 = _mm_xor_si128(MSG0, MSG2);

        /* Rounds 60-63 */
        E1 = _mm_sha1nexte_epu32(E1, MSG3);
        E0 = ABCD;
        MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
        MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
        MSG1 = _mm_xor_si128(MSG1, MSG3);

        /* Rounds 64-67 */
        E0 = _mm_sha1nexte_epu32(E0, MSG0);
        E1 = ABCD;
        MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);
        MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
        MSG2 = _mm_xor_si128(MSG2, MSG0);

        /* Rounds 68-71 */
        E1 = _mm_sha1nexte_epu32(E1, MSG1);
        E0 = ABCD;
        MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
        MSG3 = _mm_xor_si128(MSG3, MSG1);

        /* Rounds 72-75 */
        E0 = _mm_sha1nexte_epu32(E0, MSG2);
        E1 = ABCD;
        MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
        ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);

        /* Rounds 76-79 */
        E1 = _mm_sha1nexte_epu32(E1, MSG3);
        E0 = ABCD;
        ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);

        /* Combine state */
        E0 = _mm_sha1nexte_epu32(E0, E0_SAVE);
        ABCD = _mm_add_epi32(ABCD, ABCD_SAVE);

        data += 64;
        length -= 64;
    }

    /* Save state - the final state is the hash */
    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);
    _mm_storeu_si128((__m128i*) out, ABCD);
    out[4] = _mm_extract_epi32(E0, 3);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s <message>\n", argv[0]);
        exit(1);
    }

    size_t len = strlen(argv[1]);
    char *in;

    /* input must have at least 64 bytes; even the "null" input has a hash */
    if (!len || len % 64 != 0)
    {
        /*
         * Padding for SHA-1:
         * 
         * if the input length isn't a multiple of 4 Bytes, the following steps are taken
         * 1. append a 1 bit to the message - in practice, 0x80 (10000000b) is added
         * 2. fill with 0'es until the length reaches a multiple of 64B
         * 3. place the original message length, in bits, in the last 8 bytes
         */
        int zeroes = 64 - ((len + 1 + 8) % 64);

        size_t new_len = len + 1 + 8 + zeroes;

        in = calloc(new_len, 1);
        if(!in)
        {
            fprintf(stderr, "calloc failed\n");
            exit(1);
        }

        memcpy(in, argv[1], len);
        in[len] = 0x80;
        len *= 8;

        /* little-endian shenanigans */
        in[new_len - 1] = len % 255;
        in[new_len - 2] = len / 255 % 255;
        in[new_len - 3] = len / 255 / 255 % 255;
        in[new_len - 4] = len / 255 / 255 / 255 % 255;
        in[new_len - 5] = len / 255 / 255 / 255 / 255 % 255;
        in[new_len - 6] = len / 255 / 255 / 255 / 255 / 255 % 255;
        in[new_len - 7] = len / 255 / 255 / 255 / 255 / 255 / 255 % 255;
        in[new_len - 8] = len / 255 / 255 / 255 / 255 / 255 / 255 / 255 % 255;
        len = new_len;  
    }
    else
    {
        in = calloc(len, 1);
        if(!in)
        {
            fprintf(stderr, "calloc failed\n");
            exit(1);
        }
        memcpy(in, argv[1], len);
    }

    uint32_t out[5];

    sha1(in, len, out);

    /* more little-endian shenanigans */
    const uint8_t b1 = (uint8_t)(out[0] >> 24);
    const uint8_t b2 = (uint8_t)(out[0] >> 16);
    const uint8_t b3 = (uint8_t)(out[0] >>  8);
    const uint8_t b4 = (uint8_t)(out[0] >>  0);
    const uint8_t b5 = (uint8_t)(out[1] >> 24);
    const uint8_t b6 = (uint8_t)(out[1] >> 16);
    const uint8_t b7 = (uint8_t)(out[1] >>  8);
    const uint8_t b8 = (uint8_t)(out[1] >>  0);
    const uint8_t b9 = (uint8_t)(out[2] >> 24);
    const uint8_t b10 = (uint8_t)(out[2] >> 16);
    const uint8_t b11 = (uint8_t)(out[2] >>  8);
    const uint8_t b12 = (uint8_t)(out[2] >>  0);
    const uint8_t b13 = (uint8_t)(out[3] >> 24);
    const uint8_t b14 = (uint8_t)(out[3] >> 16);
    const uint8_t b15 = (uint8_t)(out[3] >>  8);
    const uint8_t b16 = (uint8_t)(out[3] >>  0);
    const uint8_t b17 = (uint8_t)(out[4] >> 24);
    const uint8_t b18 = (uint8_t)(out[4] >> 16);
    const uint8_t b19 = (uint8_t)(out[4] >>  8);
    const uint8_t b20 = (uint8_t)(out[4] >>  0);

    printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
        b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20);

    free(in);

    return 0;
}

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <wmmintrin.h>  /* for AES-NI intrinsics */

static inline void do_enc_block(__m128i *m, __m128i *k)
{
	/* Encryption; the round keys are in k, the plain-text message is in m */
	/* AddRoundKey */
	*m = _mm_xor_si128(*m, k[0]);

	for(int i = 1; i < 10; i++)
		/* SubBytes + ShiftRows + MixColumns + AddRoundKey */
		*m = _mm_aesenc_si128(*m, k[i]);
	
	/* SubBytes + ShiftRows + AddRoundKey */
	*m = _mm_aesenclast_si128(*m, k[10]);
}

static inline void do_dec_block(__m128i *m, __m128i *k)
{
	/* Decryption; the round keys are in k, the plain-text message is in m */
	/* AddRoundKey */
	*m = _mm_xor_si128(*m, k[10]);

	for(int i = 1; i < 10; i++)
		/* SubBytes + ShiftRows + MixColumns + AddRoundKey */
		*m = _mm_aesdec_si128(*m, k[10 + i]);
	
	/* SubBytes + ShiftRows + AddRoundKey */
	*m = _mm_aesdeclast_si128(*m, k[0]);
}

#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon))

static __m128i aes_128_key_expansion(__m128i key, __m128i keygened)
{
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3,3,3,3));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));

	return _mm_xor_si128(key, keygened);
}

static void aes128_load_key_enc_only(uint8_t *enc_key, __m128i *key_schedule)
{
	key_schedule[0] = _mm_loadu_si128((const __m128i*) enc_key);

	key_schedule[1]  = AES_128_key_exp(key_schedule[0], 0x01);
	key_schedule[2]  = AES_128_key_exp(key_schedule[1], 0x02);
	key_schedule[3]  = AES_128_key_exp(key_schedule[2], 0x04);
	key_schedule[4]  = AES_128_key_exp(key_schedule[3], 0x08);
	key_schedule[5]  = AES_128_key_exp(key_schedule[4], 0x10);
	key_schedule[6]  = AES_128_key_exp(key_schedule[5], 0x20);
	key_schedule[7]  = AES_128_key_exp(key_schedule[6], 0x40);
	key_schedule[8]  = AES_128_key_exp(key_schedule[7], 0x80);
	key_schedule[9]  = AES_128_key_exp(key_schedule[8], 0x1B);
	key_schedule[10] = AES_128_key_exp(key_schedule[9], 0x36);
}

static void aes128_load_key(uint8_t *enc_key, __m128i *key_schedule)
{
	/* generate encryption keys */
	aes128_load_key_enc_only(enc_key, key_schedule);

	/* generate decryption keys in reverse order.
	 * k[10] is shared by last encryption and first decryption rounds
	 * k[0] is shared by first encryption round and last decryption round (and is the original user key)
	 * For some implementation reasons, decryption key schedule is NOT the encryption key schedule in reverse order
	 */
	for (int i = 9; i >= 1; i--)
		key_schedule[20 - i] = _mm_aesimc_si128(key_schedule[i]);
}

static void aes128_enc(__m128i *key_schedule, uint8_t *plain_text, uint8_t *cipher_text)
{
	/* convert from 16 bytes char array to 16 bytes vectorial register */
	__m128i m = _mm_loadu_si128((__m128i *)plain_text);

	do_enc_block(&m, key_schedule);

	/* convert from 16 bytes vectorial register to 16 bytes char array */
	_mm_storeu_si128((__m128i *)cipher_text, m);
}

static void aes128_dec(__m128i *key_schedule, uint8_t *cipher_text, uint8_t *plain_text)
{
	__m128i m = _mm_loadu_si128((__m128i *)cipher_text);

	do_dec_block(&m, key_schedule);

	_mm_storeu_si128((__m128i *)plain_text, m);
}

int main(void){
	uint8_t plain[]	  = {95, 95, 110, 111, 116, 95, 116, 104, 101, 95, 102, 108, 97, 103, 95, 95};
	uint8_t enc_key[] = {115, 116, 114, 111, 110, 103, 95, 101, 99, 114, 121, 112, 95, 107, 101, 121}; // strong_ecryp_key
	uint8_t cipher[]  = {81, 230, 243, 196, 44, 204, 250, 171, 222, 69, 137, 168, 187, 188, 71, 176};
	uint8_t computed_cipher[16];
	uint8_t computed_plain[16];
	__m128i key_schedule[20];

	aes128_load_key(enc_key, key_schedule);

	aes128_enc(key_schedule, plain, computed_cipher);

	for (int i = 0; i < 16; i++)
		printf("%d ", computed_cipher[i]);
	printf("\n");

	aes128_dec(key_schedule, cipher, computed_plain);

	for (int i = 0; i < 16; i++)
		printf("%c", (char)computed_plain[i]);
	printf("\n");

	return 0;
}
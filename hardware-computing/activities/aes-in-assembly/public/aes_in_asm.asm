section .text
	global aes128_enc
	global aes128_dec

; void aes128_enc(__m128i *key_schedule, uint8_t *plain_text, uint8_t *cipher_text)
aes128_enc:
	push		rbp
	mov		rbp, rsp

	; key_schedule -> rdi
	; plain_text   -> rsi
	; cipher_text  -> rdx
	movdqu		xmm1, [rsi]

	xor		rcx, rcx
	xor		rax, rax
	; add round key
	pxor		xmm1, [rdi + rax]
	inc		rcx
	add		rax, 16

enc_loop:
	; encryption rounds
	aesenc		xmm1, [rdi + rax]
	add		rax, 16
	inc		rcx
	cmp		rcx, 10
	jl		enc_loop

	; last encryption round
	aesenclast	xmm1, [rdi + rax]

	movdqu		[rdx], xmm1

	leave
	ret

; void aes128_dec(__m128i *key_schedule, uint8_t *cipher_text, uint8_t *plain_text)
aes128_dec:
	; TODO: implement the decryption function

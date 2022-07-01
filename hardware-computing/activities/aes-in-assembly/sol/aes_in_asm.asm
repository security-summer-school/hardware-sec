section .text
	global aes128_enc
	global aes128_dec

; void aes128_enc(__m128i *key_schedule, uint8_t *plain_text, uint8_t *cipher_text)
aes128_enc:
	push	rbp
	mov	rbp, rsp

	; key_schedule -> rdi
	; plain_text   -> rsi
	; cipher_text  -> rdx
	; load from regular memory to XMM register
	movdqu	xmm1, [rsi]

	xor	rcx, rcx
	xor	rax, rax
	; AddRoundKey
	pxor	xmm1, [rdi + rax]
	inc	rcx
	add	rax, 16

enc_loop:
	; encryption rounds
	aesenc	xmm1, [rdi + rax]
	add	rax, 16
	inc	rcx
	cmp	rcx, 10
	jl	enc_loop

	; last encryption round
	aesenclast	xmm1, [rdi + rax]

	; move from XMM register to regular memory
	movdqu	[rdx], xmm1

	leave
	ret

; void aes128_dec(__m128i *key_schedule, uint8_t *cipher_text, uint8_t *plain_text)
aes128_dec:
	push	rbp
	mov	rbp, rsp

	; key_schedule -> rdi
	; cipher_text  -> rsi
	; plain_text   -> rdx
	movdqu	xmm1, [rsi]

	; start from the 10-th key
	mov	rax, 160
	pxor	xmm1, [rdi + rax]

	mov	rcx, 1
dec_loop:
	add	rax, 16
	aesdec	xmm1, [rdi + rax]	
	inc	rcx
	cmp	rcx, 10
	jl	dec_loop

	aesdeclast	xmm1, [rdi]

	movdqu	[rdx], xmm1

	leave
	ret

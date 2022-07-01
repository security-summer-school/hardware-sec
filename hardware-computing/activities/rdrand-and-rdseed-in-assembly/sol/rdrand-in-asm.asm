section .text
	global get_rdrand64
	global get_rdrand32
	global get_rdrand16

get_rdrand64:
	rdrand	rax
	jc	get_rdrand64.succes

	xor	rax, rax
.succes:
	ret

get_rdrand32:
	rdrand	eax
	jc	get_rdrand32.succes

	xor	eax, eax
.succes:
	ret

get_rdrand16:
	rdrand	ax
	jc	get_rdrand16.succes

	xor	ax, ax
.succes:
	ret

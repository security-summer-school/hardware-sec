section .text
	global get_rdseed64
	global get_rdseed32
	global get_rdseed16

get_rdseed64:
	rdseed	rax
	jc	get_rdseed64.succes

	xor	rax, rax
.succes:
	ret

get_rdseed32:
	rdseed	eax
	jc	get_rdseed32.succes

	xor	eax, eax
.succes:
	ret

get_rdseed16:
	rdseed	ax
	jc	get_rdseed16.succes

	xor	ax, ax
.succes:
	ret

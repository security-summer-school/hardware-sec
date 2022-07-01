CC = gcc
CFLAGS = -O0 -msse2 -march=native -maes
ASM = nasm
ASMFLAGS = -f elf64

.PHONY: all

all: main

main: main.o aes_in_asm.o

aes_in_asm.o: aes_in_asm.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

main.o: main.c

clean:
	rm -f main *.o

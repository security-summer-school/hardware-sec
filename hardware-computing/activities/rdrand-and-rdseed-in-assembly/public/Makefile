CC=gcc
CFLAGS=-march=native
ASM=nasm
ASMFLAGS=-f elf64

.PHONY : all clean

all: main

main: main.o rdrand-in-asm.o rdseed-in-asm.o

main.o: main.c

rdrand-in-asm.o: rdrand-in-asm.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

rdseed-in-asm.o: rdseed-in-asm.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

run: all
	./run.sh

clean:
	rm -f *.o main

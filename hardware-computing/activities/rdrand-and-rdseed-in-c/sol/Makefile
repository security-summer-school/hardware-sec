CC=gcc
CFLAGS=-march=native

.PHONY : all

all: main

main: main.o rdrand-in-c.o rdseed-in-c.o

main.o: main.c

rdrand-in-c.o: rdrand-in-c.c

rdseed-in-c.o: rdrand-in-c.c

run: all
	./run.sh

clean:
	rm -f *.o main

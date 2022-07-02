CC = gcc
CFLAGS = -O0 -msse2 -march=native -maes
TARGET = aes_in_c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(TARGET).c

run: $(TARGET)
	./run.sh

clean:
	rm -f $(TARGET)

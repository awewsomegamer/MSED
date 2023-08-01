SOURCES := $(shell find ./src/ -type f -name '*.c')

all:
	gcc $(SOURCES) -Isrc/include -o encoder.out
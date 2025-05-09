CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -ggdb

all: jsonc
jsonc: json.c
	$(CC) $(CFLAGS) json.c -o jsonc

clean:
	rm -f jsonc

run: jsonc
	./jsonc

.PHONY: run clean

CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -ggdb

all: jsonc
hm: main.c json.c
	$(CC) $(CFLAGS) main.c json.c -o jsonc

clean:
	rm -f jsonc

run: jsonc
	./jsonc

.PHONY: run clean

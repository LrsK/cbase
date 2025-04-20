CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -ggdb

all: hm
hm: main.c hashmap.c
	$(CC) $(CFLAGS) main.c hashmap.c -o hm

clean:
	rm -f hm

run: hm
	./hm

.PHONY: run clean

CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wpedantic -std=c99 -ggdb
OPTIMIZATION_FLAGS=-O2

all: hm
hm: main.c hashmap.c
	$(CC) $(CFLAGS) main.c hashmap.c -o hm

release: CFLAGS += $(OPTIMIZATION_FLAGS)
release: hm

clean:
	rm -rf hm hm.dSYM

run: hm
	./hm

.PHONY: run release elean

CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wpedantic -std=c99 -ggdb
OPTIMIZATION_FLAGS=-O2

all: tests/hashmap_test tests/dynamic_array_test

tests/hashmap_test: tests/hashmap_test.c hashmap.c
	$(CC) $(CFLAGS) -I.. -o $@ $^

tests/dynamic_array_test: tests/dynamic_array_test.c dynamic_array.c ../memory/arena.c
	$(CC) $(CFLAGS) -I.. -o $@ $^

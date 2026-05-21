CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wpedantic -std=c99 -ggdb
OPTIMIZATION_FLAGS=-O2

all: tests/array_test

tests/array_test: tests/array_test.c arena.c
	$(CC) $(CFLAGS) -I.. -o $@ $^



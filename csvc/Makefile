CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -ggdb

all: csvc
hm: main.c csv.c
	$(CC) $(CFLAGS) main.c csv.c -o csvc

clean:
	rm -f csvc

run: csvc
	./csvc

.PHONY: run clean

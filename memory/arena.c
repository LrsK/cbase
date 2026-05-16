#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_SIZE 64000000

typedef struct Arena {
    char* data;
    uint64_t pos;
} Arena;

Arena* arena_make() {
    char* data_ptr = (char*)malloc(ARENA_SIZE * sizeof(char*));
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->pos = 0;
    arena->data = data_ptr;

    return arena;
}

void arena_push(Arena* arena, char* data, uint8_t size) {
    &arena->data[arena->pos] = data;
    arena->pos = arena->pos + size;
}

void read_data(Arena* arena) { printf("data: %s\n", arena->data); }

int main() {
    Arena* arena = arena_make();
    read_data(arena);
    char* data = "are you ready for some footsie?";
    arena_push(arena, data, strlen(data));
    read_data(arena);
}

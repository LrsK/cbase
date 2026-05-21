#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_SIZE 64000000

Arena* arena_init(void) {
    void* data_ptr = mmap(NULL, ARENA_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    if(data_ptr == NULL) { return NULL; }
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if(arena == NULL) { return NULL; }

    arena->pos = 0;
    arena->data = data_ptr;

    return arena;
}

void arena_destroy(Arena* arena) {
    munmap(arena->data_ptr, arena->pos);
}

void arena_push(Arena* arena, void* data, size_t amount) {
    memcpy(arena->data+arena->pos, data, amount);
    arena->pos = arena->pos + amount;
}

void read_data(Arena* arena, void* start, size_t amount, void* dest) {
  memcpy(dest, start, amount);
}

void print_data(Arena* arena) { 
  printf("data: %s\n", (char*)arena->data); 
}



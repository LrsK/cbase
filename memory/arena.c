#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory/allocator.h"

#define ARENA_SIZE 64000000

static void arena_noop(){};

Arena* arena_init(void) {
    void* data_ptr = mmap(NULL, ARENA_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    if(data_ptr == NULL) { return NULL; }
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if(arena == NULL) { return NULL; }

    arena->allocator.init = arena_init;
    arena->allocator.destroy = arena_destroy;
    arena->allocator.alloc = arena_noop;
    arena->allocator.dealloc = arena_noop;

    arena->pos = 0;
    arena->cap = ARENA_SIZE;
    arena->data = data_ptr;

    return arena;
}

void arena_destroy(Allocator** arena) {
    Arena** arena = (Allocator**)arena;
    munmap((*arena)->data_ptr, (*arena)->pos);
    free(*arena);
    *arena = NULL;
}

void arena_push(Arena* arena, void* data, size_t amount) {
    memcpy(arena->data+arena->pos, data, amount);
    arena->pos = arena->pos + amount;
}

void arena_read(Arena* arena, void* start, size_t amount, void* dest) {
  memcpy(dest, start, amount);
}

void arena_print(Arena* arena) { 
  char* buf = {0};
  read_data(arena, arena->data, 10, buf)); 
  printf("data: %s\n", (char*)buf); 
}



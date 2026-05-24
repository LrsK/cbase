#include "memory/arena.h"
#include "memory/allocator.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define ARENA_SIZE 64000000

struct Arena {
    Allocator allocator;
    void* data;
    size_t position;
    size_t capacity;
};

// Forward declare interface implementations
void* arena_alloc(Allocator*, size_t);
void arena_destroy(Allocator**);
void arena_dealloc(Allocator*, size_t);

Arena* arena_init(void) {
    void* data_ptr =
        mmap(NULL, ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (data_ptr == NULL) {
        return NULL;
    }
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (arena == NULL) {
        munmap(data_ptr, ARENA_SIZE);
        return NULL;
    }

    arena->allocator.alloc = arena_alloc;
    arena->allocator.destroy = arena_destroy;
    arena->allocator.dealloc = arena_dealloc;

    arena->position = 0;
    arena->capacity = ARENA_SIZE;
    arena->data = data_ptr;

    return arena;
}

void arena_destroy(Allocator** a) {
    Arena** arena_pp = (Arena**)a;
    Arena* arena = *arena_pp;
    munmap(arena->data, ARENA_SIZE);
    free(arena);
    *arena_pp = NULL;
}

void* arena_alloc(Allocator* a, size_t amount) {
    Arena* arena = (Arena*)a;
    while (arena->position + amount > arena->capacity) {
        a->realloc(a);
    }

    void* start = (char*)arena->data + arena->position;
    arena->position = arena->position + amount;
    return start;
}

void arena_dealloc(Allocator* a, size_t amount) {
    Arena* arena = (Arena*)a;
    if (amount > arena->position) {
        arena->position = 0;
    }
    arena->position = arena->position - amount;
}

int arena_read(Allocator* a, size_t start, size_t amount, void* dest) {
    Arena* arena = (Arena*)a;
    if (start + amount > arena->position) {
        return -1;
    }
    memcpy(dest, (char*)arena->data + start, amount);
    return 0;
}

void arena_print(Allocator* a) {
    Arena* arena = (Arena*)a;
    unsigned char buf[100] = {0};
    if (arena_read(a, 0, arena->position, buf) != -1) {
        for (size_t i = 0; i < arena->position; ++i) {
            printf("%02hhx", buf[i]);
            if (i < arena->position - 1) {
                printf(" ");
            } else {
                printf("\n");
            }
        }
    }
}

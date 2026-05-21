#ifndef ALLOCATOR_ARENA_H
#define ALLOCATOR_ARENA_H

#include "memory/allocator.h"
#include <stddef.h>

typedef struct {
    Allocator allocator;
    void* data;
    size_t position;
    size_t capacity;
} Arena;

Arena* arena_init(void);
void arena_destroy(Allocator** a);
void arena_push(Allocator* a, void* data, size_t amount);

int arena_read(Allocator* a, size_t start, size_t amount, void* dest);
void arena_print(Allocator* a);

#endif

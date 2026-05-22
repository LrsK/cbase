#ifndef ALLOCATOR_ARENA_H
#define ALLOCATOR_ARENA_H

#include "memory/allocator.h"
#include <stddef.h>

// Forward declare definition
typedef struct Arena Arena;

Arena* arena_init(void);
void arena_destroy(Allocator** a);

// For testing
int arena_read(Allocator* a, size_t start, size_t amount, void* dest);
void arena_print(Allocator* a);

#endif

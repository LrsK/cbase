#ifndef ALLOCATOR_INTERFACE_H
#define ALLOCATOR_INTERFACE_H

#include <stddef.h>

typedef struct Allocator Allocator;

struct Allocator {
    Allocator* (*init)(void);
    void (*destroy)(Allocator*);
    void (*alloc)(Allocator*, size_t amount);
    void (*dealloc)(Allocator*, void* start, size_t amount);
}

#endif

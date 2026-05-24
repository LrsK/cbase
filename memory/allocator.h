#ifndef ALLOCATOR_INTERFACE_H
#define ALLOCATOR_INTERFACE_H

#include <stddef.h>

typedef struct Allocator Allocator;

struct Allocator {
    /*
     * Initialize the allocator with starting values
     * */
    void* (*init)(void);

    /*
     * Destroy all allocations
     * */
    void (*destroy)(Allocator**);

    /*
     * Allocate a new piece of amount bytes
     * */
    void* (*alloc)(Allocator*, size_t amount);

    /*
     * Grow address space
     * */
    void* (*realloc)(Allocator*);

    /*
     * Remove some amount of bytes from start point
     * */
    void (*dealloc)(Allocator*, size_t amount);
};

#endif

#include "memory/allocator.h"
#include "memory/arena.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 1000

typedef struct MyArr {
    Allocator* a;
    void* data;
    size_t capacity;
    size_t length;
} MyArr;

MyArr* dynarr_init(Allocator* a) {
    MyArr* arr = (MyArr*)malloc(sizeof(MyArr));

    arr->a = a;
    arr->length = 0;
    arr->capacity = INIT_SIZE;
    arr->data = a->alloc(a, INIT_SIZE);

    return arr;
}

void dynarr_push(MyArr* arr, void* item, size_t item_size) {
    void* dst = arr->a->alloc(arr->a, item_size);
    memcpy(dst, item, item_size);
    arr->length = arr->length + item_size;
}

int main(void) {
    Arena* arena = arena_init();
    MyArr* arr = dynarr_init((Allocator*)arena);
    char* str = "This is a string of some length";
    dynarr_push(arr, &str, strlen(str));
    arena_print((Allocator*)arena);
}

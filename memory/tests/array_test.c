#include "memory/allocator.h"
#include "memory/arena.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MyArr {
    Allocator* a;
    void* data;
    size_t length;
    size_t item_size;
} MyArr;

MyArr* dynarr_init(Allocator* a, size_t item_size) {
    MyArr* arr = (MyArr*)malloc(sizeof(MyArr));

    arr->a = a;
    arr->data = a->alloc(a, 0);
    arr->length = 0;
    arr->item_size = item_size;

    return arr;
}

void dynarr_push(MyArr* arr, void* item) {
    void* dst = arr->a->alloc(arr->a, arr->item_size);
    memcpy(dst, item, arr->item_size);
    arr->length++;
}

void dynarr_print(MyArr* arr) {
    printf("[");

    for (size_t i = 0; i < arr->length; ++i) {
        if (i < arr->length - 1) {
            printf("%x, ", *((unsigned char*)arr->data + (i * arr->item_size)));
            continue;
        }
        printf("%x", *((unsigned char*)arr->data + (i * arr->item_size)));
    }

    printf("]\n");
}

int main(void) {
    Arena* arena = arena_init();
    MyArr* arr = dynarr_init((Allocator*)arena, sizeof(int));
    for (int i = 0; i < 10; ++i) {
        dynarr_push(arr, &i);
    }
    arena_print((Allocator*)arena);

    dynarr_print(arr);
}

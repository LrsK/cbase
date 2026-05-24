#include "collections/dynamic_array.h"
#include "memory/allocator.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_DEFAULT_ALLOC_SIZE 2

DynamicArray* dynarr_make(Allocator* a, size_t item_size) {
    DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (arr == NULL) {
        return NULL;
    }
    arr->allocator = a;
    arr->length = 0;
    arr->item_size = item_size;
    arr->data = a->alloc(a, 0);

    return arr;
}

int dynarr_get(DynamicArray* arr, size_t index, void* dest) {
    if (index >= arr->length) {
        return -1;
    }

    memcpy(dest, (unsigned char*)arr->data + (index * arr->item_size), arr->item_size);
    return 0;
}

int dynarr_push(DynamicArray* arr, const void* items, size_t num) {
    void* dest = arr->allocator->alloc(arr->allocator, num * arr->item_size);
    if (dest == NULL) {
        return -1;
    }

    memcpy(dest, items, num * arr->item_size);

    arr->length = arr->length + num;
    return 0;
}

int dynarr_pop(DynamicArray* arr, size_t num) {
    if (num > arr->length) {
        return -1;
    }

    arr->allocator->dealloc(arr->allocator, num * arr->item_size);
    arr->length = arr->length - num;

    return 0;
}

int dynarr_extend(DynamicArray* arr, DynamicArray* extention) {
    void* dest = arr->allocator->alloc(arr->allocator, extention->length * extention->item_size);
    if (dest == NULL) {
        return -1;
    }

    memcpy(dest, extention->data, extention->length * extention->item_size);
    arr->length = arr->length + extention->length;
    return 0;
}

int dynarr_set(DynamicArray* arr, size_t index, void* item) {
    if (index > arr->length - 1) {
        return -1;
    }

    if (item == NULL) {
        return -1;
    }

    memcpy((unsigned char*)arr->data + index * arr->item_size, item, arr->item_size);

    return 0;
}

void dynarr_print(DynamicArray* arr, void (*fn_print_item)(void* item)) {
    printf("[");
    for (size_t i = 0; i < arr->length; ++i) {
        fn_print_item((char*)arr->data + i * arr->item_size);
        if (i < arr->length - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

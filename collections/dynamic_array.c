#include "collections/dynamic_array.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_DEFAULT_ALLOC_SIZE 2

DynamicArray* dynarr_init(size_t item_size) {
    void* alloc_data = malloc(DA_DEFAULT_ALLOC_SIZE * item_size);
    if (alloc_data == NULL) {
        return NULL;
    }
    DynamicArray* new_array = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (new_array == NULL) {
        free(alloc_data);
        return NULL;
    }
    new_array->length = 0;
    new_array->capacity = DA_DEFAULT_ALLOC_SIZE;
    new_array->item_size = item_size;
    new_array->data = alloc_data;

    return new_array;
}

void dynarr_destroy(DynamicArray* arr, void (*fn_destroy_item)(void*)) {
    if (fn_destroy_item != NULL) {
        for (size_t i = 0; i < arr->length; ++i) {
            fn_destroy_item((char*)arr->data + i * arr->item_size);
        }
    }
    free(arr->data);
    arr->data = NULL;
    free(arr);
}

static int dynarr_resize(DynamicArray* arr) {
    void* new_mem = realloc(arr->data, arr->capacity * 2 * arr->item_size);
    if (new_mem == NULL) {
        return -1;
    }
    arr->data = new_mem;
    arr->capacity = arr->capacity * 2;
    return 0;
}

int dynarr_get(DynamicArray* arr, size_t index, void* dst) {
    if (index >= arr->length) {
        return -1;
    }

    memcpy(dst, (unsigned char*)arr->data + index * arr->item_size, arr->item_size);
    return 0;
}

int dynarr_push(DynamicArray* arr, const void* items, size_t num) {
    while (arr->length + num > arr->capacity) {
        if (dynarr_resize(arr) == -1) {
            return -1;
        };
    }

    memcpy((unsigned char*)arr->data + arr->length * arr->item_size, items, num * arr->item_size);

    arr->length = arr->length + num;
    return 0;
}

int dynarr_pop(DynamicArray* arr, size_t num) {
    if (num > arr->length) {
        num = arr->length;
    }
    
    arr->length = arr->length - num;
    
    return 0;
}

int dynarr_extend(DynamicArray *arr, DynamicArray *extention) {
    while (arr->length + extention->length > arr->capacity) {
        if(dynarr_resize(arr) == -1) {
            return -1;
        }
    }

    void* end = (unsigned char*)arr->data+arr->item_size*arr->length;
    size_t n = extention->length * extention->item_size;
    memcpy(end, extention->data, n);
    arr->length = arr->length + extention->length;
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

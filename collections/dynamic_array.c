#include "collections/dynamic_array.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_DEFAULT_ALLOC_SIZE 2

DynamicArray array_init(size_t item_size) {
    void* alloc_data = malloc(DA_DEFAULT_ALLOC_SIZE * item_size);
    if (alloc_data == NULL) {
        printf("Fatal error: unable to allocate memory\n");
        exit(1);
    }

    DynamicArray return_array = {
        .length = 0,
        .capacity = DA_DEFAULT_ALLOC_SIZE,
        .item_size = item_size,
        .data = alloc_data,
    };

    return return_array;
}

static void array_resize(DynamicArray* array) {
    void* new_mem = realloc(array->data, array->capacity * 2 * array->item_size);
    if (new_mem == NULL) {
        printf("Fatal error: unable to allocate memory\n");
        exit(1);
    }
    array->data = new_mem;
    array->capacity = array->capacity * 2;
}

void array_append(DynamicArray* array, const void* items, size_t num) {
    while (array->length + num > array->capacity) {
        array_resize(array);
    }

    memcpy((unsigned char*)array->data + array->length * array->item_size, items,
           num * array->item_size);

    array->length = array->length + num;
}

void array_print(DynamicArray* array, void (*fn_print_item)(void* item)) {
    printf("[");
    for (size_t i = 0; i < array->length; ++i) {
        fn_print_item((char*)array->data + i * array->item_size);
        if (i < array->length - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void array_destroy(DynamicArray* array, void (*fn_destroy_item)(void*)) {
    if (fn_destroy_item != NULL) {
        for (size_t i = 0; i < array->length; ++i) {
            fn_destroy_item((char*)array->data + array->length*array->item_size);
        }
    }
    free(array->data);
    array->data = NULL;
}

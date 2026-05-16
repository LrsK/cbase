#include <stddef.h>

typedef struct DynamicArray {
    size_t length;
    size_t capacity;
    size_t item_size;
    void* data;
} DynamicArray;

DynamicArray array_init(size_t item_size);
void array_append(DynamicArray* array, const void* items, size_t num);
void array_print(DynamicArray* array, void (*fn_print_item)(void* item));
void array_destroy(DynamicArray* array, void (*fn_destroy_item)(void*));

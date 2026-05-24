#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "memory/allocator.h"
#include <stddef.h>

typedef size_t (*CompareFunc)(void* a, void* b);

typedef struct DynamicArray {
    Allocator* allocator;
    size_t length;
    size_t item_size;
    void* data;
} DynamicArray;

/**
 * @brief Allocate and initialize a new DynamicArray.
 *
 * @param a The allocator to use for memory management.
 * @param item_size Size in bytes of each element.
 * @return Pointer to the new array, or NULL on allocation failure.
 */
DynamicArray* dynarr_make(Allocator* a, size_t item_size);

/**
 * @brief Copy the element at index into dst.
 *
 * @param arr The array to read from.
 * @param index The position of the element to retrieve.
 * @param dst Destination buffer of at least item_size bytes.
 * @return 0 on success, -1 if index is out of bounds.
 */
int dynarr_get(DynamicArray* arr, size_t index, void* dst);

/**
 * @brief Append num elements to the end of the array.
 *
 * @param arr The array to push into.
 * @param items Pointer to the elements to append.
 * @param num Number of elements to append.
 * @return 0 on success, -1 on allocation failure.
 */
int dynarr_push(DynamicArray* arr, const void* items, size_t num);

/**
 * @brief Remove num elements from the end of the array.
 *        Clamped to the current length if num exceeds it.
 *
 * @param arr The array to pop from.
 * @param num Number of elements to remove.
 * @return 0 on success.
 */
int dynarr_pop(DynamicArray* arr, size_t num);

/**
 * @brief Append all elements of extention to arr.
 *
 * @param arr The array to extend.
 * @param extention The array whose elements are appended.
 * @return 0 on success, -1 on allocation failure.
 */
int dynarr_extend(DynamicArray* arr, DynamicArray* extention);

/*
 * @brief Set the value of item at position index
 * @param index Position where item will be set
 * @param item Item containing data to be set at index
 * @return 0 on success, -1 on error
 *
 */
int dynarr_set(DynamicArray* arr, size_t index, void* item);

/*
 * @brief Find an item in the array
 * @param item Item you want to find in the array
 * @return the index of the item when found or -1 if not found
 *
 */
int dynarr_find(DynamicArray* arr, void* item, CompareFunc cmp);

/*
 * @brief Return a DynamicArray that reflects the content of another
 * @param from Where to begin the slice
 * @param to Where to end the slice
 * @return A pointer to a new DynamicArray
 *
 */
DynamicArray* dynarr_slice(DynamicArray* arr, size_t from, size_t to);

/**
 * @brief Print all elements using a caller-provided print function.
 *
 * @param arr The array to print.
 * @param fn_print_item Callback invoked for each element.
 */
void dynarr_print(DynamicArray* arr, void (*fn_print_item)(void* item));

#endif

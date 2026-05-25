#include "collections/dynamic_array.h"
#include "memory/arena.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define ASSERT(cond)                                                                               \
    if (!(cond)) {                                                                                 \
        printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);                                    \
        failures++;                                                                                \
    }

// ── int helpers ──────────────────────────────────────────────────────────────

void print_int(void* n) {
    printf("%d", *(int*)n);
}

static size_t compare_int(void* a, void* b) {
    return *(int*)a == *(int*)b;
}

// ── int tests ─────────────────────────────────────────────────────────────────

static void test_push_get(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(int));

    int val = 42;
    dynarr_push(arr, &val, 1);

    int out;
    ASSERT(dynarr_get(arr, 0, &out) == 0);
    ASSERT(out == 42);
    ASSERT(arr->length == 1);
    ASSERT(dynarr_get(arr, 1, &out) == -1);

    arr->allocator->destroy(&arr->allocator);
}

static void test_push_multiple_pop_get(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(int));

    int val = 42;
    dynarr_push(arr, &val, 1);
    int val2 = 4555;
    dynarr_push(arr, &val2, 1);
    int val3 = 123123123;
    dynarr_push(arr, &val3, 1);

    int out;
    ASSERT(dynarr_get(arr, 0, &out) == 0);
    ASSERT(out == 42);
    ASSERT(dynarr_get(arr, 1, &out) == 0);
    ASSERT(out == 4555);
    ASSERT(dynarr_get(arr, 2, &out) == 0);
    ASSERT(out == 123123123);
    ASSERT(arr->length == 3);
    ASSERT(dynarr_get(arr, 55555555, &out) == -1);
    ASSERT(dynarr_pop(arr, -11) == -1);
    ASSERT(dynarr_pop(arr, 11) == -1);
    ASSERT(dynarr_pop(arr, 1) == 0);
    ASSERT(dynarr_get(arr, 2, &out) == -1);
    ASSERT(dynarr_get(arr, 1, &out) == 0);
    ASSERT(out == 4555);
    ASSERT(arr->length == 2);
    int val4 = 8675309;
    dynarr_push(arr, &val4, 1);
    ASSERT(dynarr_get(arr, 2, &out) == 0);
    ASSERT(out == val4);
}

static void test_set(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(int));

    int val = 1;
    dynarr_push(arr, &val, 1);
    val = 2;
    dynarr_push(arr, &val, 1);
    val = 3;
    dynarr_push(arr, &val, 1);

    int replacement = 99;
    ASSERT(dynarr_set(arr, 1, &replacement) == 0);
    ASSERT(dynarr_set(arr, 55555555, &replacement) == -1);

    int out;
    ASSERT(dynarr_get(arr, 0, &out) == 0);
    ASSERT(out == 1);
    ASSERT(dynarr_get(arr, 1, &out) == 0);
    ASSERT(out == 99);
    ASSERT(dynarr_get(arr, 2, &out) == 0);
    ASSERT(out == 3);
    ASSERT(arr->length == 3);

    arr->allocator->destroy(&arr->allocator);
}

static void test_find_int(void) {
    Arena* aa = arena_init();
    DynamicArray* int_arr = dynarr_make((Allocator*)aa, sizeof(int));

    int val = 1;
    dynarr_push(int_arr, &val, 1);
    val = 2;
    dynarr_push(int_arr, &val, 1);
    val = 3;
    dynarr_push(int_arr, &val, 1);

    int search_val = 99;
    ASSERT(dynarr_find(int_arr, &search_val, compare_int) == -1);
    search_val = 2;
    ASSERT(dynarr_find(int_arr, &search_val, compare_int) == 1);
}

// ── MyString helpers ──────────────────────────────────────────────────────────

typedef struct {
    size_t length;
    char* characters;
} MyString;

MyString* make_my_string(char* cstr) {
    MyString* str = (MyString*)malloc(sizeof(MyString));
    str->characters = (char*)malloc(sizeof(char) * strlen(cstr));
    str->length = strlen(cstr);
    memcpy(str->characters, cstr, str->length);
    return str;
}

void destroy_my_string(void* str) {
    free(((MyString*)str)->characters);
}

size_t equal_my_strings(void* a, void* b) {
    if (((MyString*)a)->length != ((MyString*)b)->length)
        return 0;
    for (size_t i = 0; i < ((MyString*)a)->length; ++i) {
        if (((MyString*)a)->characters[i] != ((MyString*)b)->characters[i])
            return 0;
    }
    return 1;
}

void print_my_string(void* s) {
    for (size_t i = 0; i < ((MyString*)s)->length; ++i) {
        printf("%c", ((MyString*)s)->characters[i]);
    }
}

// ── MyString tests ────────────────────────────────────────────────────────────

static void test_push_multiple_struct_get(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(MyString));

    Arena* aa2 = arena_init();
    DynamicArray* ext = dynarr_make((Allocator*)aa2, sizeof(MyString));

    char* test_cstr = "TestString";
    char* test_cstr2 = "OtherString!11";
    MyString* test_str = make_my_string(test_cstr);
    MyString* test_str2 = make_my_string(test_cstr2);

    for (int i = 0; i < 10; ++i) {
        MyString* str = make_my_string(test_cstr);
        dynarr_push(arr, str, 1);
        free(str);
    }

    for (int i = 0; i < 20; ++i) {
        MyString* str = make_my_string(test_cstr2);
        dynarr_push(ext, str, 1);
        free(str);
    }

    MyString out_str = {0};
    ASSERT(dynarr_get(arr, 0, &out_str) == 0);
    ASSERT(equal_my_strings(test_str, &out_str));
    ASSERT(dynarr_get(arr, 1, &out_str) == 0);
    ASSERT(equal_my_strings(test_str, &out_str));
    ASSERT(!equal_my_strings(test_str2, &out_str));
    ASSERT(dynarr_get(arr, 2, &out_str) == 0);
    ASSERT(equal_my_strings(test_str, &out_str));
    ASSERT(arr->length == 10);
    ASSERT(dynarr_get(arr, 55555555, &out_str) == -1);

    dynarr_extend(arr, ext);

    ASSERT(dynarr_get(arr, 0, &out_str) == 0);
    ASSERT(equal_my_strings(test_str, &out_str));
    ASSERT(dynarr_get(arr, 10, &out_str) == 0);
    ASSERT(equal_my_strings(test_str2, &out_str));
    ASSERT(dynarr_get(arr, 29, &out_str) == 0);
    ASSERT(equal_my_strings(test_str2, &out_str));
    ASSERT(arr->length == 30);
    ASSERT(dynarr_get(arr, 55555555, &out_str) == -1);

    arr->allocator->destroy(&arr->allocator);
    ext->allocator->destroy(&ext->allocator);
}

static void test_find_struct(void) {
    Arena* aa = arena_init();
    DynamicArray* str_arr = dynarr_make((Allocator*)aa, sizeof(MyString));

    MyString* my_str = make_my_string("This is the first string");
    dynarr_push(str_arr, my_str, 1);
    my_str = make_my_string("This is the second string");
    dynarr_push(str_arr, my_str, 1);
    my_str = make_my_string("This is the third string");
    dynarr_push(str_arr, my_str, 1);

    MyString* search_val = make_my_string("This is the wrong string");
    ASSERT(dynarr_find(str_arr, search_val, equal_my_strings) == -1);
    search_val = make_my_string("This is the second string");
    ASSERT(dynarr_find(str_arr, search_val, equal_my_strings) == 1);

    str_arr->allocator->destroy(&str_arr->allocator);
}

// ── allocator tests ───────────────────────────────────────────────────────────

static void test_slice(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(int));

    for (int i = 0; i < 10; ++i) {
        dynarr_push(arr, &i, 1);
    }

    DynamicArray* slice = dynarr_slice(arr, 3, 7);
    ASSERT(slice != NULL);
    ASSERT(slice->length == 4);
    ASSERT(slice->allocator == NULL);

    int out;
    ASSERT(dynarr_get(slice, 0, &out) == 0);
    ASSERT(out == 3);
    ASSERT(dynarr_get(slice, 3, &out) == 0);
    ASSERT(out == 6);
    ASSERT(dynarr_get(slice, 4, &out) == -1);

    ASSERT(dynarr_slice(arr, 5, 3) == NULL);
    ASSERT(dynarr_slice(arr, 5, 5) == NULL);
    ASSERT(dynarr_slice(arr, 0, 11) == NULL);

    ASSERT(dynarr_push(slice, &out, 1) == -1);

    free(slice);
    arr->allocator->destroy(&arr->allocator);
}

static void test_destroy(void) {
    Arena* aa = arena_init();
    DynamicArray* arr = dynarr_make((Allocator*)aa, sizeof(int));

    int val = 42;
    dynarr_push(arr, &val, 1);

    arr->allocator->destroy(&arr->allocator);
    ASSERT(arr->allocator == NULL);
}

// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    test_push_get();
    test_push_multiple_pop_get();
    test_set();
    test_find_int();
    test_push_multiple_struct_get();
    test_find_struct();
    test_slice();
    test_destroy();
    if (failures == 0)
        printf("All tests passed.\n");
    else
        printf("%d test(s) failed.\n", failures);
}

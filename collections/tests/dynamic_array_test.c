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
}

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

int equal_my_strings(MyString* a, MyString* b) {
    if (a->length != b->length)
        return 0;
    for (size_t i = 0; i < a->length; ++i) {
        if (a->characters[i] != b->characters[i])
            return 0;
    }
    return 1;
}

void print_my_string(void* s) {
    for (size_t i = 0; i < ((MyString*)s)->length; ++i) {
        printf("%c", ((MyString*)s)->characters[i]);
    }
}

void destroy_my_string(void* str) {
    free(((MyString*)str)->characters);
}

void print_struct_item(void* thing) {
    size_t length = ((MyString*)thing)->length;
    char* chars = ((MyString*)thing)->characters;
    printf("{ length: %zu, str: %.*s }", length, (int)length, chars);
}

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

int main(void) {
    test_push_get();
    test_push_multiple_struct_get();
    test_push_multiple_pop_get();
    if (failures == 0)
        printf("All tests passed.\n");
    else
        printf("%d test(s) failed.\n", failures);
}

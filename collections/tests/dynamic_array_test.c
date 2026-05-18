#include "collections/dynamic_array.h"
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
    DynamicArray* arr = dynarr_init(sizeof(int));

    int val = 42;
    dynarr_push(arr, &val, 1);

    int out;
    ASSERT(dynarr_get(arr, 0, &out) == 0);
    ASSERT(out == 42);
    ASSERT(arr->length == 1);
    ASSERT(dynarr_get(arr, 1, &out) == -1);

    dynarr_destroy(arr, NULL);
}

static void test_push_multiple_get(void) {
    DynamicArray* arr = dynarr_init(sizeof(int));
    // DynamicArray* ext = dynarr_init(sizeof(int));

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

    dynarr_destroy(arr, NULL);
}

typedef struct {
    size_t length;
    char* characters;
} MyString;

MyString* make_my_string(void) {
    MyString* str = (MyString*)malloc(sizeof(MyString));
    size_t length = 10;
    str->characters = (char*)malloc(sizeof(char) * length);
    str->length = length;
    memcpy(str->characters, "TestString", length);
    return str;
}

MyString* make_my_string2(void) {
    MyString* str = (MyString*)malloc(sizeof(MyString));
    size_t length = 14;
    str->characters = (char*)malloc(sizeof(char) * length);
    str->length = length;
    memcpy(str->characters, "OtherString!!!", length);
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

void destroy_my_string(void* str) {
    free(((MyString*)str)->characters);
}

void print_struct_item(void* thing) {
    size_t length = ((MyString*)thing)->length;
    char* chars = ((MyString*)thing)->characters;
    printf("{ length: %zu, str: %.*s }", length, (int)length, chars);
}

static void test_push_multiple_struct_get(void) {
    DynamicArray* arr = dynarr_init(sizeof(MyString));
    DynamicArray* ext = dynarr_init(sizeof(MyString));

    for (int i = 0; i < 10; ++i) {
        MyString* str = make_my_string();
        dynarr_push(arr, str, 1);
        free(str);
    }

    for (int i = 0; i < 20; ++i) {
        MyString* str = make_my_string2();
        dynarr_push(ext, str, 1);
        free(str);
    }

    MyString out_str = {0};
    MyString test_str = {.characters = "TestString", .length = 10};
    MyString test_str2 = {.characters = "OtherString!!!", .length = 14};
    ASSERT(dynarr_get(arr, 0, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str, &out_str));
    ASSERT(dynarr_get(arr, 1, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str, &out_str));
    ASSERT(dynarr_get(arr, 2, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str, &out_str));
    ASSERT(arr->length == 10);
    ASSERT(dynarr_get(arr, 55555555, &out_str) == -1);

    dynarr_extend(arr, ext);

    ASSERT(dynarr_get(arr, 0, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str, &out_str));
    ASSERT(dynarr_get(arr, 10, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str2, &out_str));
    ASSERT(dynarr_get(arr, 29, &out_str) == 0);
    ASSERT(equal_my_strings(&test_str2, &out_str));
    ASSERT(arr->length == 30);
    ASSERT(dynarr_get(arr, 55555555, &out_str) == -1);

    dynarr_destroy(arr, NULL);
    dynarr_destroy(ext, NULL);
}

int main(void) {
    test_push_get();
    test_push_multiple_get();
    test_push_multiple_struct_get();
    if (failures == 0)
        printf("All tests passed.\n");
    else
        printf("%d test(s) failed.\n", failures);
}

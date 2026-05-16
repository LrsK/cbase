#include "collections/dynamic_array.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_str_item(void* str) {
    printf("\"%s\"", *(char**)str);
}

void print_int_item(void* number) {
    printf("%d", *(int*)number);
}

typedef struct {
    size_t length;
    char* characters;
} MyString;

MyString* make_my_string(void) {
    MyString* str = (MyString*)malloc(sizeof(MyString));
    str->characters = (char*)malloc(sizeof(char) * 10);
    str->length = 10;
    memcpy(str->characters, "TestString", 10);
    return str;
}

void destroy_my_string(void* str) {
    free(((MyString*)str)->characters);
}

void print_struct_item(void* thing) {
    size_t length = ((MyString*)thing)->length;
    char* chars = ((MyString*)thing)->characters;
    printf("{ length: %zu, str: %s }", length, chars);
}

int main(void) {
    // Integers
    DynamicArray int_array = array_init(sizeof(int));
    for (int i = 0; i < 1000; ++i) {
        array_append(&int_array, &i, 1);
    }
    array_print(&int_array, print_int_item);
    array_destroy(&int_array, NULL);

    // Pointer array
    DynamicArray string_array = array_init(sizeof(char*));
    const char* myStr = "This is a string!";
    for (int i = 0; i < 1000; ++i) {
        array_append(&string_array, &myStr, 1);
    }
    array_print(&string_array, print_str_item);
    array_destroy(&string_array, NULL);

    // Structs
    DynamicArray struct_array = array_init(sizeof(MyString));
    for (int i = 0; i < 1000; ++i) {
        MyString* str = make_my_string();
        array_append(&struct_array, str, 1);
    }
    array_print(&struct_array, print_struct_item);
    array_destroy(&struct_array, destroy_my_string);
}

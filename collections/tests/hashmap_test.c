#include "collections/hashmap.h"
#include "memory/allocator.h"
#include "memory/arena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define ASSERT(cond)                                                                               \
    if (!(cond)) {                                                                                 \
        printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);                                    \
        failures++;                                                                                \
    }

// ── helpers ───────────────────────────────────────────────────────────────────

#define ITEMS 1000000
#define NUMPADDING 4

static void str_shift_left(char* str, uint8_t amount) {
    int len = strlen(str) + 1;
    memmove(str, str + amount, len - amount);
}

static char** make_strs(void) {
    char** strs = malloc(ITEMS * sizeof(char*));
    if (strs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ITEMS; ++i) {
        strs[i] = malloc(10 * sizeof(char));
        if (strs[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        sprintf(strs[i], "key%0*d", NUMPADDING, i);
    }
    return strs;
}

static void print_str_str(void* key, void* val) {
    printf("%s: %s", *(char**)key, *(char**)val);
}

// ── tests ─────────────────────────────────────────────────────────────────────

static void test_set_get_del(void) {
    Arena* aa = arena_init();
    HashMap* hm = hashmap_make((Allocator*)aa, sizeof(char*), sizeof(char*), hashmap_hash_str,
                               hashmap_cmp_str, 0);

    char result[100];

    ASSERT(hashmap_set(hm, "test123", "test123") == 0);
    ASSERT(hashmap_get(hm, "test123", result) == 0);
    ASSERT(hashmap_del(hm, "test123") == 0);
    ASSERT(hashmap_get(hm, "test123", result) != 0);

    hashmap_destroy(hm);
    arena_destroy((Allocator**)&aa);
}

static void test_set_get_large(void) {
    Arena* aa = arena_init();
    HashMap* hm = hashmap_make((Allocator*)aa, sizeof(char*), sizeof(char*), hashmap_hash_str,
                               hashmap_cmp_str, 0);

    char** keys = make_strs();
    char** values = make_strs();

    for (int i = 0; i < ITEMS; ++i) {
        hashmap_set(hm, keys[i], values[i]);
    }

    char result[10];
    for (int i = 0; i < ITEMS; ++i) {
        ASSERT(hashmap_get(hm, keys[i], result) == 0);
        str_shift_left(keys[i], 3);
        str_shift_left(values[i], 3);
        ASSERT(strcmp(keys[i], values[i]) == 0);
    }

    hashmap_destroy(hm);
    arena_destroy((Allocator**)&aa);
}

static void test_print_destroy(void) {
    Arena* aa = arena_init();
    HashMap* hm = hashmap_make((Allocator*)aa, sizeof(char*), sizeof(char*), hashmap_hash_str,
                               hashmap_cmp_str, 0);

    hashmap_set(hm, "test123", "test123");
    hashmap_set(hm, "my123", "qwerty");
    hashmap_set(hm, "vvvv", "cccc");
    hashmap_print(hm, print_str_str);
    printf("\n");
    hashmap_destroy(hm);

    arena_destroy((Allocator**)&aa);
}

// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    test_set_get_del();
    test_set_get_large();
    test_print_destroy();

    if (failures == 0) {
        printf("All tests passed.\n");
    } else {
        printf("%d test(s) failed.\n", failures);
    }
}

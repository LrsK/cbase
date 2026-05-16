#include "collections/hashmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_shift_left(char* str, uint8_t amount) {
    int len = strlen(str) + 1;
    memmove(str, str + amount, len - amount);
}

size_t str_hash(void* key) {
    uint32_t hash = HASHMAP_FNV_32_BIT_OFFSET_BASIS;
    for (int i = 0; ((char*)key)[i] != '\0'; ++i) {
        char keystr = ((char*)key)[i];
        hash = hash ^ keystr;
        hash = hash * HASHMAP_FNV_32_BIT_PRIME;
    }

    return hash;
}

size_t str_cmp(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}

void print_str_str_hashmap(KV* slot) {
    printf("%s: %s", (char*)slot->key, (char*)slot->value);
}

void destroy_str_str_hashmap(KV* slot) {
    free(slot->key);
    free(slot->value);
}

#define ITEMS 1000000
#define NUMPADDING 4

// Quick and dirty string generation
char** make_strs(void) {
    char** strs = malloc(ITEMS * sizeof(char*));
    if (strs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each string in the dynamic arrays
    for (int i = 0; i < ITEMS; ++i) {
        strs[i] = malloc(10 * sizeof(char)); // Allocate space for each key
        if (strs[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        // Populate the keys and values
        sprintf(strs[i], "key%0*d", NUMPADDING, i);
    }

    return strs;
}

int main(void) {
    HashMap hm = hashmap_init(sizeof(char*), sizeof(char*), str_hash, str_cmp);

    char** keys = make_strs();
    char** values = make_strs();

    // Set data
    for (int i = 0; i < ITEMS; ++i) {
        hashmap_set(&hm, keys[i], values[i]);
    }

    char result[100];
    for (int i = 0; i < ITEMS; ++i) {
        hashmap_get(&hm, keys[i], result);
        str_shift_left(keys[i], 3);
        str_shift_left(values[i], 3);
        if (strcmp(keys[i], values[i])) {
            printf("%s:%s - different.\n", keys[i], result);
        }
    }

    int status = 0;
    hashmap_set(&hm, "test123", "test123");
    // hashmap_print(hm);
    status = hashmap_get(&hm, "test123", result);
    if (status) {
        printf("something went wrong when looking for test123\n");
    }
    status = hashmap_del(&hm, "test123");
    if (status) {
        printf("something went wrong when deleting test123\n");
    }
    status = hashmap_get(&hm, "test123", result);
    if (!status) {
        printf("shouldn't have found test123\n");
    }
    hashmap_destroy(&hm, NULL);

    HashMap hm2 = hashmap_init(sizeof(char*), sizeof(char*), str_hash, str_cmp);
    hashmap_set(&hm2, "test123", "test123");
    hashmap_set(&hm2, "my123", "qwerty");
    hashmap_set(&hm2, "vvvv", "cccc");
    hashmap_print(&hm2, print_str_str_hashmap);
    hashmap_destroy(&hm2, destroy_str_str_hashmap);

}

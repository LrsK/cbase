#include "hashmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t hash(const char* key) {
    uint32_t hash = HASHMAP_FNV_32_BIT_OFFSET_BASIS;

    for (int i = 0; key[i] != '\0'; ++i) {
        hash = hash ^ key[i];
        hash = hash * HASHMAP_FNV_32_BIT_PRIME;
    }

    return hash;
}

HashMap* hashmap_init(void) {
    HashMap* hm = calloc(1, sizeof(HashMap));
    KV* slots = calloc(HASHMAP_INIT_SLOT_SIZE, sizeof(KV));
    hm->slots = slots;
    hm->size = HASHMAP_INIT_SLOT_SIZE;

    return hm;
}

static void hashmap_resize(HashMap* hm) {
    size_t original_size = hm->size;
    hm->size = hm->size * HASHMAP_SLOT_GROWTH_FACTOR;
    printf("RESIZE %zu->%zu\n", original_size, hm->size);
    KV* new_slots = (KV*)calloc(hm->size, sizeof(KV));

    // Rehash
    for (size_t i = 0; i < original_size; ++i) {
        if (hm->slots[i].set) {
            uint32_t index = hash(hm->slots[i].key) % hm->size;
            while (new_slots[index].set) {
                index = (index + 1) % hm->size;
            }
            new_slots[index].key = hm->slots[i].key; // Transfer ownership
            new_slots[index].value = hm->slots[i].value; // Transfer ownership
            new_slots[index].set = 1; // Mark as set	
        }
    }
    free(hm->slots);
    hm->slots = new_slots;
}

void hashmap_set(HashMap* hm, const char* key, const char* value) {
    uint32_t index = hash(key) % hm->size;
    uint32_t org_index = index;

    while (hm->slots[index].set && strcmp(hm->slots[index].key, key)) {
        // move to next slot, this is a collision
        index = (index + 1) % hm->size;
        if (index == org_index) {
            // We looped around, resize
            hashmap_resize(hm);
        }
    }

    // Insert
    hm->slots[index].key = calloc(strlen(key) + 1, sizeof(char));
    strcpy(hm->slots[index].key, key);

    hm->slots[index].value = calloc(strlen(value) + 1, sizeof(char));
    strcpy(hm->slots[index].value, value);

    hm->slots[index].set = 1;
}

int hashmap_get(HashMap* hm, const char* key, char* result) {
    uint32_t index = hash(key) % hm->size;
    uint32_t lookup_index = index;
    do {
        if (hm->slots[lookup_index].set && strcmp(hm->slots[lookup_index].key, key) == 0) {
            strcpy(result, hm->slots[lookup_index].value);
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->size;
    } while (lookup_index != index);

    return -1;
}

int hashmap_del(HashMap *hm, const char *key) {
    uint32_t index = hash(key) % hm->size;
    uint32_t lookup_index = index;
    do {
        if (hm->slots[lookup_index].set && strcmp(hm->slots[lookup_index].key, key) == 0) {
            free(hm->slots[lookup_index].key);
            hm->slots[lookup_index].key = 0;
            free(hm->slots[lookup_index].value);
            hm->slots[lookup_index].value = 0;
            hm->slots[lookup_index].set = 0;
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->size;
    } while (lookup_index != index);

    return -1;
}

void hashmap_print(HashMap* hm) {
    for (size_t i = 0; i < hm->size; ++i) {
        printf("%s: %s\n", hm->slots[i].key, hm->slots[i].value);
    }
}

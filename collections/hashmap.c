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

HashMap hashmap_init(size_t key_size, size_t val_size, HashFn hash, CmpFn cmp) {
    KV* slots = (KV*)calloc(HASHMAP_INIT_SLOT_SIZE, sizeof(KV));
    HashMap hm = {
        .key_size = key_size,
        .val_size = val_size,
        .hash = hash,
        .cmp = cmp,
        .set_count = 0,
        .capacity = HASHMAP_INIT_SLOT_SIZE,
        .slots = slots,
    };

    return hm;
}

static void hashmap_resize(HashMap* hm) {
    size_t original_capacity = hm->capacity;
    hm->capacity = hm->capacity * HASHMAP_SLOT_GROWTH_FACTOR;
    printf("RESIZE %zu->%zu\n", original_capacity, hm->capacity);
    KV* new_slots = (KV*)calloc(hm->capacity, sizeof(KV));

    // Rehash
    for (size_t i = 0; i < original_capacity; ++i) {
        if (hm->slots[i].set) {
            uint32_t index = hm->hash(hm->slots[i].key) % hm->capacity;
            while (new_slots[index].set) {
                index = (index + 1) % hm->capacity;
            }
            new_slots[index].key = hm->slots[i].key;     // Transfer ownership
            new_slots[index].value = hm->slots[i].value; // Transfer ownership
            new_slots[index].set = 1;                    // Mark as set
        }
    }
    free(hm->slots);
    hm->slots = new_slots;
}

void hashmap_set(HashMap* hm, void* key, void* value) {
    if (hm->set_count > ((hm->capacity * 3) / 4)) {
        hashmap_resize(hm);
    }

    uint32_t index = hm->hash(key) % hm->capacity;
    uint32_t org_index = index;

    while (hm->slots[index].set && hm->cmp(hm->slots[index].key, key)) {
        // move to next slot, this is a collision
        index = (index + 1) % hm->capacity;
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
    hm->set_count++;
}

int hashmap_get(HashMap* hm, void* key, void* result) {
    uint32_t index = hm->hash(key) % hm->capacity;
    uint32_t lookup_index = index;
    do {
        if (hm->slots[lookup_index].set && hm->cmp(hm->slots[lookup_index].key, key) == 0) {
            strcpy(result, hm->slots[lookup_index].value);
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->capacity;
    } while (lookup_index != index);

    return -1;
}

int hashmap_del(HashMap* hm, void* key) {
    uint32_t index = hash(key) % hm->capacity;
    uint32_t lookup_index = index;
    do {
        if (hm->slots[lookup_index].set && hm->cmp(hm->slots[lookup_index].key, key) == 0) {
            free(hm->slots[lookup_index].key);
            hm->slots[lookup_index].key = 0;
            free(hm->slots[lookup_index].value);
            hm->slots[lookup_index].value = 0;
            hm->slots[lookup_index].set = 0;
            hm->set_count--;
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->capacity;
    } while (lookup_index != index);

    return -1;
}

void hashmap_print(HashMap* hm, void (*fn_print_hashmap)(KV*)) {
    printf("{");
    for (size_t i = 0; i < hm->capacity; ++i) {
        fn_print_hashmap(&hm->slots[i]);
        if (i < hm->capacity - 1) {
            printf(", ");
        }
    }
    printf("}");
}

void hashmap_destroy(HashMap* hm, void (*fn_hashmap_item_destroy)(KV* slot)) {
    if (fn_hashmap_item_destroy) {
        for (size_t i = 0; i < hm->capacity ; ++i) {
            if (hm->slots[i].key != NULL) {
                fn_hashmap_item_destroy(&hm->slots[i]);
            }
        }
    }
    free(hm->slots);
}

uint32_t hashmap_num_keys(HashMap* hm) {
    return hm->set_count;
}

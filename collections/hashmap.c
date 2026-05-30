#include "hashmap.h"
#include "memory/allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t hashmap_hash_str(void* key) {
    uint32_t hash = HASHMAP_FNV_32_BIT_OFFSET_BASIS;
    for (char* p = (char*)key; *p != '\0'; ++p) {
        hash ^= (uint8_t)*p;
        hash *= HASHMAP_FNV_32_BIT_PRIME;
    }
    return hash;
}

size_t hashmap_cmp_str(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}

HashMap* hashmap_make(Allocator* a, size_t key_size, size_t val_size, HashFn hash, CmpFn cmp,
                      size_t alignment) {
    HashMap* hm = (HashMap*)malloc(sizeof(HashMap));

    if (alignment == 0) {
        alignment = sizeof(void*);
    }
    hm->allocator = a;
    hm->key_size = key_size;
    hm->val_size = val_size;
    // 1 byte is for set flag, alignment set to make memory reads faster
    hm->stride = (key_size + val_size + 1 + alignment - 1) & ~(alignment - 1);
    hm->hash = hash;
    hm->cmp = cmp;
    hm->num_slots = HASHMAP_INIT_SLOT_SIZE;
    hm->num_set_slots = 0;
    hm->slots = a->alloc(a, HASHMAP_INIT_SLOT_SIZE * hm->stride);

    return hm;
}

static void hashmap_resize(HashMap* hm) {
    size_t prev_num_slots = hm->num_slots;

    // Save current
    void* current_slots = malloc(prev_num_slots * hm->stride);
    memcpy(current_slots, hm->slots, prev_num_slots * hm->stride);

    hm->num_slots = hm->num_slots * HASHMAP_SLOT_GROWTH_FACTOR;
    printf("RESIZE %zu->%zu\n", prev_num_slots, hm->num_slots);
    hm->allocator->alloc(hm->allocator, (hm->num_slots - prev_num_slots) * hm->stride);

    // Clear
    memset((unsigned char*)hm->slots, 0, hm->num_slots * hm->stride);

    // Rehash
    for (size_t i = 0; i < prev_num_slots; ++i) {
        void* current_slot = (unsigned char*)current_slots + i * hm->stride;
        uint8_t* i_set = (uint8_t*)current_slot;

        if (*i_set) { // the set flag
            void* i_key = (unsigned char*)current_slot + 1;
            uint32_t new_index = hm->hash(i_key) % hm->num_slots;
            uint8_t* n_set = (uint8_t*)hm->slots + new_index * hm->stride;
            // If already set, find next available slot
            while (*n_set) {
                new_index = (new_index + 1) % hm->num_slots;
                n_set = (uint8_t*)hm->slots + new_index * hm->stride;
            }
            // is set
            *n_set = 1;
            // key
            memcpy((unsigned char*)hm->slots + (new_index * hm->stride) + 1,
                   (unsigned char*)current_slots + (i * hm->stride) + 1, hm->key_size);
            // value
            memcpy((unsigned char*)hm->slots + (new_index * hm->stride) + 1 + hm->key_size,
                   (unsigned char*)current_slots + (i * hm->stride) + 1 + hm->key_size,
                   hm->val_size);
        }
    }
    free(current_slots);
}

int hashmap_set(HashMap* hm, void* key, void* value) {
    if (hm->num_set_slots > ((hm->num_slots * 3) / 4)) {
        hashmap_resize(hm);
    }

    uint32_t index = hm->hash(key) % hm->num_slots;

    void* i_slot = (unsigned char*)hm->slots + index * hm->stride;
    uint8_t* i_set = (uint8_t*)i_slot;
    void* i_key = (unsigned char*)i_slot + 1;

    uint32_t org_index = index;
    while (*i_set && hm->cmp(i_key, key) == 0) {
        // move to next slot, this is a benign collision
        index = (index + 1) % hm->num_slots;
        if (index == org_index) {
            // We looped around, resize
            hashmap_resize(hm);
        }
        i_slot = (unsigned char*)hm->slots + index * hm->stride;
        i_set = (uint8_t*)i_slot;
        i_key = (unsigned char*)i_slot + 1;
    }

    // Insert
    *i_set = 1;
    // key
    memcpy(i_key, key, hm->key_size);
    // value
    memcpy((unsigned char*)i_key + hm->key_size, value, hm->val_size);

    hm->num_set_slots++;

    return 0;
}

int hashmap_get(HashMap* hm, void* key, void* result) {
    uint32_t index = hm->hash(key) % hm->num_slots;
    uint32_t lookup_index = index;
    do {
        void* i_slot = (unsigned char*)hm->slots + lookup_index * hm->stride;
        uint8_t* i_set = (uint8_t*)i_slot;
        void* i_key = (unsigned char*)i_slot + 1;
        if (*i_set && hm->cmp(i_key, key) == 0) {
            memcpy(result, (unsigned char*)i_key + hm->key_size, hm->val_size);
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->num_slots;
    } while (lookup_index != index);

    return -1;
}

int hashmap_del(HashMap* hm, void* key) {
    uint32_t index = hm->hash(key) % hm->num_slots;
    uint32_t lookup_index = index;
    do {
        void* i_slot = (unsigned char*)hm->slots + lookup_index * hm->stride;
        uint8_t* i_set = (uint8_t*)i_slot;
        void* i_key = (unsigned char*)i_slot + 1;
        if (*i_set && hm->cmp(i_key, key) == 0) {
            // Clear everything here
            memset(i_slot, 0, hm->key_size + hm->val_size + 1);
            hm->num_set_slots--;
            return 0;
        }
        lookup_index = (lookup_index + 1) % hm->num_slots;
    } while (lookup_index != index);

    return -1;
}

void hashmap_print(HashMap* hm, void (*fn_print_hashmap)(void* key, void* val)) {
    printf("{");
    for (size_t i = 0; i < hm->num_slots; ++i) {
        void* slot = (unsigned char*)hm->slots + (i * hm->stride);
        fn_print_hashmap((unsigned char*)slot + 1, (unsigned char*)slot + 1 + hm->key_size);
        if (i < hm->num_slots - 1) {
            printf(", ");
        }
    }
    printf("}");
}

void hashmap_destroy(HashMap* hm) {
    hm->allocator->destroy(&hm->allocator);
    free(hm);
}

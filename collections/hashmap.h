#ifndef HASHMAP_H
#define HASHMAP_H

#include "memory/allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_INIT_SLOT_SIZE 32
#define HASHMAP_SLOT_GROWTH_FACTOR 2
#define HASHMAP_FNV_32_BIT_PRIME 16777619
#define HASHMAP_FNV_32_BIT_OFFSET_BASIS 2166136261

typedef size_t (*HashFn)(void* key);
typedef size_t (*CmpFn)(void* a, void* b);

typedef struct {
    void* key;
    void* value;
    uint8_t set;
} KV;

typedef struct {
    Allocator* allocator;
    HashFn hash;
    CmpFn cmp;
    size_t key_size;
    size_t val_size;
    size_t num_slots;
    size_t num_set_slots;
    size_t capacity;
    KV* slots;
} HashMap;

size_t hashmap_hash_str(void* key);
size_t hashmap_hash_bytes(void* key, size_t len);

size_t hashmap_cmp_str(void* a, void* b);
size_t hashmap_cmp_bytes(void* a, void* b, size_t len);

HashMap hashmap_init_str(size_t val_size);
HashMap hashmap_init_num(size_t key_size, size_t val_size);


HashMap* hashmap_init(Allocator* a, size_t key_size, size_t val_size, HashFn hash, CmpFn cmp);
void hashmap_destroy(HashMap* hm, void (*fn_hashmap_item_destroy)(KV*));

void hashmap_set(HashMap* hm, void* key, void* value);
int hashmap_get(HashMap* hm, void* key, void* result);
int hashmap_del(HashMap* hm, void* key);

void hashmap_print(HashMap* hm, void(*fn_print_hashmap)(KV*));

#endif

#ifndef HASHMAP_H
#define HASHMAP_H

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
    HashFn hash;
    CmpFn cmp;
    size_t key_size;
    size_t val_size;
    size_t set_count;
    size_t capacity;
    KV* slots;
} HashMap;

HashMap hashmap_init(size_t key_size, size_t val_size, HashFn hash, CmpFn cmp);
void hashmap_set(HashMap* hm, void* key, void* value);
int hashmap_get(HashMap* hm, void* key, void* result);
int hashmap_del(HashMap* hm, void* key);
void hashmap_print(HashMap* hm, void(*fn_print_hashmap)(KV*));
void hashmap_destroy(HashMap* hm, void (*fn_hashmap_item_destroy)(KV*));

#endif

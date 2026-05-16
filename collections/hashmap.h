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

typedef struct {
    char* key;
    char* value;
    uint8_t set;
} KV;

typedef struct {
    size_t size;
    size_t set_count;
    KV* slots;
} HashMap;

HashMap* hashmap_init(void);
void hashmap_set(HashMap* hm, const char* key, const char* value);
int hashmap_get(HashMap* hm, const char* key, char* result);
int hashmap_del(HashMap* hm, const char* key);
void hashmap_print(HashMap* hm);

#endif

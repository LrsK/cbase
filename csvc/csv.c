#include <time.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOC_AMOUNT 10
#define INITIAL_HASHMAP_SIZE 32
#define HASHMAP_GROWTH_FACTOR 3

// Used in FNV-1a hash func
static uint32_t FNV_offset_basis = 2166136261;
static uint32_t FNV_prime = 16777619;

typedef enum { OUTSIDE, DATA, POSSIBLE_ESCAPED_QUOTE } ParseMode;
typedef uint8_t bool;

typedef struct {
  char* key;
  char* value;
  bool set;
} KV;

typedef struct { 
  size_t size;
  KV* items;
} HashMap;

uint32_t hash(char* value) {
  uint32_t hash = FNV_offset_basis;
  for (int i = 0; value[i] != '\0'; ++i) {
    hash = hash ^ (uint32_t)value[i];
    hash = hash * FNV_prime;
  }
  return hash;
}

HashMap* hashmap_init() {
  HashMap* hm = (HashMap*)malloc(sizeof(HashMap));
  KV* items = (KV*)malloc(sizeof(KV)*INITIAL_HASHMAP_SIZE);
  hm->size = INITIAL_HASHMAP_SIZE;
  for (int i = 0; i < INITIAL_HASHMAP_SIZE; ++i) {
    items[i].key = NULL;
    items[i].value = NULL;
    items[i].set = 0;
  }
  hm->items = items;
  return hm;
}

void hashmap_resize(HashMap* hm) {
  size_t new_size = hm->size * HASHMAP_GROWTH_FACTOR;
  printf("RESIZING! new size: %ld\n", new_size);
  hm->items = realloc(hm->items, sizeof(KV)*new_size);
  for (size_t i = hm->size; i < new_size; ++i) {
    hm->items[i].key = NULL;
    hm->items[i].value = NULL;
    hm->items[i].set = 0;
  }
  hm->size = new_size;
  
  // Rehash
  char* key = 0;
  char* value = 0;
  uint32_t index = 0;
  uint32_t index_step = 0;
  uint32_t lookup_index = 0;

  for (size_t i = 0; i < hm->size; ++i) {
    if (hm->items[i].set) {
      key = hm->items[i].key;
      value = hm->items[i].value;
      index = hash(key) % hm->size;

      lookup_index = (index+index_step) % hm->size;
      while (!hm->items[lookup_index].set) {
        // The new location is occupied
        index_step++;
        lookup_index = (index+index_step) % hm->size;
        if (lookup_index == index) {
          // wrap around! expand again!
          hashmap_resize(hm);
        }
      }
      hm->items[lookup_index].key = (char*)realloc(hm->items[lookup_index].key, sizeof(char)*(strlen(key)+1));
      hm->items[lookup_index].value = (char*)realloc(hm->items[lookup_index].value, sizeof(char)*(strlen(value)+1));
      strcpy(hm->items[lookup_index].key, key);
      strcpy(hm->items[lookup_index].value, value);
      hm->items[lookup_index].set = 1;
      index_step = 0;
    }
  }
}

int hashmap_get(HashMap* hm, char* key, char* result) {
  uint32_t index = hash(key) % hm->size;
  uint32_t index_step = 0;
  uint32_t lookup_index = (index + index_step) % hm->size;
  
  do {
    if (hm->items[lookup_index].set == 0) {
      index_step++;
      lookup_index = (index + index_step) % hm->size;
      continue;
      //*result = 0;
      //printf("not found because set is zero\n");
      //return -1;
    }
    
    if (strcmp(hm->items[lookup_index].key, key) == 0) {
      strcpy(result, hm->items[lookup_index].value);
      return 0;
    }

    index_step++;
    lookup_index = (index + index_step) % hm->size;
  } while (lookup_index != index);

  printf("not found because looked through everything\n");
  *result = 0;
  return -1;
}

int hashmap_set(HashMap* hm, char* key, char* value) {
  uint32_t index = hash(key) % hm->size;
  uint32_t index_step = 0;
  uint32_t lookup_index = (index + index_step) % hm->size;
  while (hm->items[lookup_index].set != 0) {
    printf("COLLISION! %s\n", key);
    if (strcmp(hm->items[lookup_index].key, key) == 0) {
      return -1;
    }
    index_step++;
    lookup_index = (index + index_step) % hm->size;
    if (lookup_index == index) {
      // We tried all the positions, expand size
      hashmap_resize(hm);
      lookup_index = hm->size+1;
      for (size_t i = 0; i < hm->size; ++i) {
        printf("new - %s:%s\n", hm->items[i].key, hm->items[i].value);
      }
      break;
    }
  }

  hm->items[lookup_index].key = (char*)malloc(sizeof(char)*(strlen(key)+1));
  hm->items[lookup_index].value = (char*)malloc(sizeof(char)*(strlen(value)+1));

  strcpy(hm->items[lookup_index].key, key);
  strcpy(hm->items[lookup_index].value, value);
  hm->items[lookup_index].set = 1;
  
  return 0;
}

typedef struct {
  char* data;
  size_t length;
} String;

String* string_init(const char* cstr) {
  size_t len = strlen(cstr);
  String* str = (String*)malloc(sizeof(String));
  char* strdata = (char*)malloc(sizeof(char)*len);

  for (int c = 0; cstr[c] != '\0'; ++c) {
    strdata[c] = cstr[c];
  }
  str->data = strdata;
  str->length = len;
  return str;
}

void print_string(String* str) {
  for (size_t i = 0; i < str->length; ++i) {
    printf("%c", str->data[i]);
  }
}

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
    unsigned int n_columns;
    unsigned int n_lines;
    char delimiter;
    char quote;
    ParseMode mode;
} CSV;


CSV* csv_init(char delimiter, char quote) {
    CSV* aptr = (CSV*)malloc(sizeof(CSV));
    aptr->length = 0;
    aptr->capacity = ALLOC_AMOUNT;
    aptr->data = (char*)malloc(sizeof(char)*ALLOC_AMOUNT);
    aptr->delimiter = delimiter;
    aptr->quote = quote;
    aptr->n_columns = 1;
    aptr->n_lines = 0;
    aptr->mode = OUTSIDE;

    return aptr;
}

void csv_append(CSV* csv, char ch) {
    if (csv->length + 1 >= csv->capacity) {
      int new_cap = csv->capacity*2;
      csv->data = realloc(csv->data, sizeof(char)*new_cap);
      csv->capacity = new_cap;
    }

    csv->data[csv->length+1] = ch;
    csv->length = csv->length + 1;
    
    if (ch == csv->quote) {
      if (csv->mode == OUTSIDE) {
        csv->mode = DATA;
      } else if (csv->mode == DATA) {
        csv->mode = POSSIBLE_ESCAPED_QUOTE;
      } else if (csv->mode == POSSIBLE_ESCAPED_QUOTE) {
        csv->mode = DATA;
      }
    } else {
      if (csv->mode == POSSIBLE_ESCAPED_QUOTE) {
        csv->mode = OUTSIDE;
      }
    }

    switch (csv->mode) {
      case OUTSIDE:
        if (ch == '\n') {
          csv->n_lines++;
        } else if (ch == csv->delimiter) {
          if (!csv->n_lines) {
            csv->n_columns++;
          }
        }
        break;
      case DATA:
        break;
      case POSSIBLE_ESCAPED_QUOTE:
        break;
    }
}




int main() {
  FILE *fp = fopen("file.csv", "r");
  if (fp == NULL) {
    printf("Error opening file: %d\n", errno);
    return 1;
  }

  CSV* csv_data = csv_init(',', '"');

  char c;
  while ((c = fgetc(fp)) != EOF) {
    csv_append(csv_data, c);
  }

  printf("length: %ld, num lines: %d, num columns: %d\n", csv_data->length, csv_data->n_lines, csv_data->n_columns);
  String str = {.data = csv_data->data, .length = csv_data->length };
  print_string(&str);
  printf("\n");
  HashMap* hm = hashmap_init();

  // Insert a lot of stuff
  //
  int num_strs = 100;
  char keys[num_strs][6];
  for (int i = 0; i < num_strs; ++i) {
    sprintf(keys[i], "key%d", i);
  }

  char values[num_strs][8];
  for (int i = 0; i < num_strs; ++i) {
    sprintf(values[i], "value%d", i);
  }

  for (int i = 0; i < num_strs; ++i) {
    hashmap_set(hm, keys[i], values[i]);
    //printf("%s\n", keys[i]);
    //printf("%s\n", values[i]);
  }

  for (size_t i = 0; i < hm->size; ++i) {
    printf("%s: %s - %d\n", hm->items[i].key, hm->items[i].value, hm->items[i].set);
  }

  char result[100];

  for (int i = 0; i < num_strs; ++i) {
    if(hashmap_get(hm, keys[i], result)==0) {
      printf("found key: %s has value: %s\n", keys[i], result);
    } else {
      printf("%s not found\n", keys[i]);
    }
    result[0] = 0;
  }

  return 0;
}

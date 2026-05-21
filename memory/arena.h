#include "memory/allocator.h"

typedef struct {
  Allocator allocator;
  void* data;
  size_t pos;
  size_t cap;
} Arena;

Arena* arena_init(void);
void arena_destroy(Allocator** arena);
void arena_push(Allocator* arena, void* data, size_t amount); 
void arena_read(Allocator* arena, void* start, size_t amount, void* dest);
void arena_print(Allocator* arena);


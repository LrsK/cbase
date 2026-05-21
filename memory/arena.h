#include "allocator.h"

typedef struct {
  Allocator allocator;
  void* data;
  size_t pos;
  size_t cap;
} Arena;

Arena* arena_init(void);
void arena_destroy(Arena*);
void arena_push(Arena* arena, void* data, size_t amount); 
void read_data(Arena* arena, void* dest);


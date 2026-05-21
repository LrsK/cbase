#include "memory/arena.h"

int main(void) {
    Arena* arena = arena_init();
    arena_print(arena);
    char* data = "are you ready for some footsie?";
    arena_push(arena, data, strlen(data));
    arena_print(arena);
    arena_destroy(&arena);
}

#include "memory/arena.h"

int main(void) {
    Arena* arena = arena_init();
    print_data(arena);
    char* data = "are you ready for some footsie?";
    arena_push(arena, data, strlen(data));
    read_data(arena);
}

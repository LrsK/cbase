#include "hashmap.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void str_shift_left(char* str, uint8_t amount) {
    int len = strlen(str) + 1;
    memmove(str, str + amount, len - amount);
}

int main() {
    HashMap* hm = hashmap_init();
#define ITEMS 4000
#define NUMPADDING 4
    char keys[ITEMS][10];
    for (int i = 0; i < ITEMS; ++i) {
        sprintf(keys[i], "key%0*d", NUMPADDING, i);
    }

    char values[ITEMS][10];
    for (int i = 0; i < ITEMS; ++i) {
        sprintf(values[i], "value%0*d", NUMPADDING, i);
    }

    for (int i = 0; i < ITEMS; ++i) {
        hashmap_set(hm, keys[i], values[i]);
    }

    hashmap_print(hm);

    char result[100];
    for (int i = 0; i < ITEMS; ++i) {
        hashmap_get(hm, keys[i], result);
        str_shift_left(keys[i], 3);
        str_shift_left(values[i], 5);
        if (strcmp(keys[i], values[i])) {
            printf("%s:%s - different.\n", keys[i], result);
        }
    }
}

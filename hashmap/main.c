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
#define ITEMS 10
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

    int status = 0;
    hashmap_set(hm, "test123", "test123");
    hashmap_print(hm);
    status = hashmap_get(hm, "test123", result);
    if (status) {
        printf("something went wrong when looking for test123\n");
    }
    status = hashmap_del(hm, "test123");
    if (status) {
        printf("something went wrong when deleting test123\n");
    }
    status = hashmap_get(hm, "test123", result);
    if (!status) {
        printf("shouldn't have found test123\n");
    }
    hashmap_print(hm);

}

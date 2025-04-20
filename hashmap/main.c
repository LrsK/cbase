#include "hashmap.h"

int main() {
    HashMap* hm = hashmap_init();
#define ITEMS 4000
    char keys[ITEMS][10];
    for (int i = 0; i < ITEMS; ++i) {
        sprintf(keys[i], "key%02d", i);
    }

    char values[ITEMS][10];
    for (int i = 0; i < ITEMS; ++i) {
        sprintf(values[i], "value%02d", i);
    }

    for (int i = 0; i < ITEMS; ++i) {
        hashmap_set(hm, keys[i], values[i]);
    }

    hashmap_print(hm);

    char result[100];
    for (int i = 0; i < ITEMS; ++i) {
        hashmap_get(hm, keys[i], result);

        if (keys[i][strlen(keys[i]) - 1] != result[strlen(result) - 1] ||
            keys[i][strlen(keys[i]) - 2] != result[strlen(result) - 2]) {
            printf("%s:%s - different.\n", keys[i], result);
        }
    }
}

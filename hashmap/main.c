#include "hashmap.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void str_shift_left(char* str, uint8_t amount) {
    int len = strlen(str) + 1;
    memmove(str, str + amount, len - amount);
}

int main(void) {
    HashMap* hm = hashmap_init();
#define ITEMS 1000000
#define NUMPADDING 4

    char **keys = malloc(ITEMS * sizeof(char *));
    char **values = malloc(ITEMS * sizeof(char *));
    
    // Check for allocation failure
    if (keys == NULL || values == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Allocate memory for each string in the dynamic arrays
    for (int i = 0; i < ITEMS; ++i) {
        keys[i] = malloc(10 * sizeof(char)); // Allocate space for each key
        values[i] = malloc(10 * sizeof(char)); // Allocate space for each value

        // Check for allocation failure
        if (keys[i] == NULL || values[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            // Free previously allocated memory before exiting
            for (int j = 0; j < i; ++j) {
                free(keys[j]);
                free(values[j]);
            }
            free(keys);
            free(values);
            return 1;
        }

        // Populate the keys and values
        sprintf(keys[i], "key%0*d", NUMPADDING, i);
        sprintf(values[i], "val%0*d", NUMPADDING, i);
    }

    for (int i = 0; i < ITEMS; ++i) {
        hashmap_set(hm, keys[i], values[i]);
    }

    //hashmap_print(hm);

    char result[100];
    for (int i = 0; i < ITEMS; ++i) {
        hashmap_get(hm, keys[i], result);
        str_shift_left(keys[i], 3);
        str_shift_left(values[i], 3);
        if (strcmp(keys[i], values[i])) {
            printf("%s:%s - different.\n", keys[i], result);
        }
    }

    int status = 0;
    hashmap_set(hm, "test123", "test123");
    //hashmap_print(hm);
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
    //hashmap_print(hm);

}

#include "hashmap.h"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALLOC_AMOUNT 10
#define INITIAL_HASHMAP_SIZE 32
#define HASHMAP_GROWTH_FACTOR 3

typedef enum { OUTSIDE, DATA, POSSIBLE_ESCAPED_QUOTE } ParseMode;
typedef uint8_t bool;

typedef struct {
    HashMap* hm;
    unsigned int n_columns;
    unsigned int n_lines;
    char delimiter;
    char quote;
    ParseMode mode;
} CSV;

CSV* csv_init(char delimiter, char quote) {
    CSV* aptr = (CSV*) malloc(sizeof(CSV));
    aptr->hm = hashmap_init();
    aptr->delimiter = delimiter;
    aptr->quote = quote;
    aptr->n_columns = 1;
    aptr->n_lines = 0;
    aptr->mode = OUTSIDE;

    return aptr;
}

void csv_append(CSV* csv, char ch) {

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
    case DATA: printf("%c", ch); break;
    case POSSIBLE_ESCAPED_QUOTE: break;
    }
}

int main() {
    FILE* fp = fopen("file.csv", "r");
    if (fp == NULL) {
        printf("Error opening file: %d\n", errno);
        return 1;
    }

    CSV* csv_data = csv_init(',', '"');

    char c;
    while ((c = fgetc(fp)) != EOF) {
        csv_append(csv_data, c);
    }

    fclose(fp);

    return 0;
}

#include "hashmap.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;

typedef struct {
    char* data;
    unsigned int capacity;
    unsigned int len;
} String;

typedef struct {
    char* data;
    unsigned int len;
} StringSlice;

char* StringSliceGet(StringSlice* slice) {
    char* realStr = malloc(slice->len + 1);
    for (unsigned int i = 0; i < slice->len; i++) {
        realStr[i] = slice->data[i];
    }
    realStr[slice->len] = '\0';
    return realStr;
}

String* StringNew(unsigned int capacity) {
    String* s = malloc(sizeof(String));
    s->data = malloc(capacity + 1);
    s->data[0] = '\0';
    s->capacity = capacity;
    s->len = 0;
    return s;
}

void StringAppend(String* s, char c) {
    if (s->len >= s->capacity - 1) {
        s->capacity *= 2;
        s->data = realloc(s->data, s->capacity);
    }
    s->data[s->len] = c;
    s->len++;
    s->data[s->len] = '\0';
}

char* StringGet(String* s) {
    return s->data;
}

typedef struct DynArray {
    void** items;
    unsigned int capacity;
    unsigned int len;
} DynArray;

DynArray* dynarray_make(void* arr, size_t size) {
    DynArray* da = malloc(sizeof(DynArray));
    da->items = malloc(size*capacity);
    da->items = arr;
    da->capacity = 1;
    da->len = 0;
    return da;
}

void dynarray_append(DynArray* da, void* item, size_t size) {
    if (da->len >= da->capacity - 1) {
        da->capacity *= 2;
        da->items = realloc(da->items, da->capacity);
    }
    da->items[da->len] = item;
    da->len += size;
    da->items[da->len] = '\0';
}

// Define JSON tokens
enum JSONTokenKind {
    OBJECT_START,
    OBJECT_END,
    ARRAY_START,
    ARRAY_END,
    COMMA,
    COLON,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NONE,
    END,
    ILLEGAL
};

typedef struct JSONToken {
    enum JSONTokenKind kind;
    char* value;
    unsigned int pos;
} JSONToken;

typedef struct JSONLexer {
    const char* data;
    unsigned int len;
    unsigned int pos;
    unsigned int read_pos;
    char current_char;
} JSONLexer;

static char json_lexer_peek(JSONLexer* lexer) {
    if (lexer->read_pos >= lexer->len) {
        return EOF;
    }
    return lexer->data[lexer->read_pos];
}

static char json_lexer_read(JSONLexer* lexer) {
    lexer->current_char = json_lexer_peek(lexer);
    lexer->pos = lexer->read_pos;
    lexer->read_pos++;
    return lexer->current_char;
}

static void json_lexer_skip_whitespace(JSONLexer* lexer) {
    while (lexer->current_char == ' ' || lexer->current_char == '\t' || lexer->current_char == '\n' ||
           lexer->current_char == '\r') {
        json_lexer_read(lexer);
    }
}

static int json_lexer_is_literal(JSONLexer* lexer, const char* literal) {
    for (int i = 0; literal[i] != '\0'; i++) {
        if (lexer->current_char != literal[i]) {
            return 0;
        }
        json_lexer_read(lexer);
    }
    return 1;
}

int lexer_init(JSONLexer* lexer, const char* data, unsigned int len) {
    lexer->data = data;
    lexer->len = len;
    lexer->pos = 0;
    lexer->read_pos = 0;
    lexer->current_char = 0;
    json_lexer_read(lexer);
    return 0;
}

int json_lexer_next(JSONLexer* lexer, JSONToken* token) {
    json_lexer_skip_whitespace(lexer);

    unsigned int pos = lexer->pos;
    if (lexer->current_char == EOF) {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = END, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == '{') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = OBJECT_START, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == '}') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = OBJECT_END, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == '[') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = ARRAY_START, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == ']') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = ARRAY_END, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == ',') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = COMMA, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == ':') {
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = COLON, .value = NULL, .pos = pos};
        return 0;
    } else if (lexer->current_char == '"') {
        // Read string
        json_lexer_read(lexer);
        pos = lexer->pos;
        StringSlice slice = {.data = (char*) lexer->data + pos, .len = 0};
        bool escape_mode = 0;
        while (lexer->current_char != '"' || escape_mode) {
            if (lexer->current_char == '\\') {
                escape_mode = 1;
            } else {
                escape_mode = 0;
            }
            slice.len++;
            json_lexer_read(lexer);
        }
        // Eat closing quote
        json_lexer_read(lexer);
        *token = (JSONToken) {.kind = STRING, .value = StringSliceGet(&slice), .pos = pos};
        return 0;
    } else if (lexer->current_char == '-' || (lexer->current_char >= '0' && lexer->current_char <= '9')) {
        // Read number, can be int or float and negative
        String* string = StringNew(2);
        if (lexer->current_char == '-') {
            // Negative number
            StringAppend(string, lexer->current_char);
            json_lexer_read(lexer);
        }
        while (lexer->current_char >= '0' && lexer->current_char <= '9') {
            StringAppend(string, lexer->current_char);
            json_lexer_read(lexer);
        }
        if (lexer->current_char == '.') {
            StringAppend(string, lexer->current_char);
            json_lexer_read(lexer);
            while (lexer->current_char >= '0' && lexer->current_char <= '9') {
                StringAppend(string, lexer->current_char);
                json_lexer_read(lexer);
            }
        }

        *token = (JSONToken) {.kind = NUMBER, .value = StringGet(string), .pos = pos};
        return 0;

    } else if (lexer->current_char == 't') {
        // Read true
        if (!json_lexer_is_literal(lexer, "true")) {
            *token = (JSONToken) {.kind = ILLEGAL, .value = NULL, .pos = pos};
        }

        *token = (JSONToken) {.kind = TRUE, .value = "true", .pos = pos};
        return 0;

    } else if (lexer->current_char == 'f') {
        // Read false
        if (!json_lexer_is_literal(lexer, "false")) {
            *token = (JSONToken) {.kind = ILLEGAL, .value = NULL, .pos = pos};
        }
        *token = (JSONToken) {.kind = FALSE, .value = "false", .pos = pos};
        return 0;

    } else if (lexer->current_char == 'n') {
        // Read null
        if (!json_lexer_is_literal(lexer, "null")) {
            *token = (JSONToken) {.kind = ILLEGAL, .value = NULL, .pos = pos};
        }
        *token = (JSONToken) {.kind = NONE, .value = NULL, .pos = pos};
        return 0;
    }

    *token = (JSONToken) {.kind = END, .value = NULL, .pos = pos};
    return 0;
}

typedef enum {
    JSON_ELEMENT_STRING,
    JSON_ELEMENT_NUMBER,
    JSON_ELEMENT_BOOLEAN,
    JSON_ELEMENT_NULL,
    JSON_ELEMENT_ARRAY,
    JSON_ELEMENT_OBJECT,
} JSONElementKind;

typedef struct JSONElement {
    JSONElementKind kind;
    union {
        char* string;
        float number;
        bool boolean;
        DynArray array; // Array of JSONElement
        HashMap object; // Map of *char to JSONElement
    };
} JSONElement;

typedef struct JSONParser {
    JSONLexer lexer;
} JSONParser;

int json_parser_init(JSONParser* parser, JSONLexer lexer) {
    parser->lexer = lexer;

    return 0;
}

int json_parser_parse_array(JSONParser* parser, JSONElement* element) {
    int result = 0;

    element->kind = JSON_ELEMENT_ARRAY; 
    element->array = *dynarray_make(2);
    return result;
}

int json_parser_parse_object(JSONParser* parser, JSONElement* element) {
    int result = 0;

    return result;
}

int json_parser_parse(JSONParser* parser, JSONElement* element) {
    int result = 0;

    JSONToken token = {0};

    if (json_lexer_next(&parser->lexer, &token) != 0) {
        return -1;
    }

    if (token.kind == OBJECT_START) {
        result = json_parser_parse_object(parser, element);
    } else if (token.kind == ARRAY_START) {
        result = json_parser_parse_array(parser, element);
    } else if (token.kind == STRING) {
        element->kind = JSON_ELEMENT_STRING;
        element->string = token.value;
    } else if (token.kind == NUMBER) {
        element->kind = JSON_ELEMENT_NUMBER;
        element->number = atof(token.value);
     } else if (token.kind == TRUE) {
        element->kind = JSON_ELEMENT_BOOLEAN;
        element->boolean = 1;
    } else if (token.kind == FALSE) {
        element->kind = JSON_ELEMENT_BOOLEAN;
        element->boolean = 0;
    } else if (token.kind == NONE) {
        element->kind = JSON_ELEMENT_NULL;
        element->boolean = 0; 

    } else if (token.kind == FALSE) {
        element->kind = JSON_ELEMENT_BOOLEAN;
        element->boolean = 0;
    } else if (token.kind == FALSE) {
        element->kind = JSON_ELEMENT_BOOLEAN;
        element->boolean = 0;    } else {
        return -1;
    }

    if (token.kind != END) {
        return -1;
    }
}




void json_parser_free(JSONParser* parser);

int main(int argc, char* argv[]) {
    // Parse arguments
    if (argc < 2) {
        printf("Usage: %s <JSON file>\n", argv[0]);
        return 1;
    }

    // Load data from file in args
    FILE* file = fopen("test.json", "r"); // argv[1], "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", argv[1]);
        return 1;
    }

    // Read data from file
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = malloc(file_size + 1);
    const size_t read_ret = fread(data, 1, file_size, file);
    if (read_ret != file_size) {
        fclose(file);
        return 1;
    }
    data[file_size] = '\0';

    JSONLexer lexer = {0};
    JSONParser parser = {0};
    JSONToken token = {0};
    JSONElement element = {0};

    printf("JSON file size: %zu bytes\n", file_size);
    printf("JSON data: %s\n", data);

    // Close file
    fclose(file);
    // exit(1);

    // Initialize lexer
    lexer_init(&lexer, data, file_size);
    json_parser_init(&parser, lexer);

    json_parser_parse(&parser, &element);

    // Parse JSON data
//    do {
//        token = json_lexer_next(&lexer);
//        switch (token.kind) {
//        case OBJECT_START: printf("{\n"); break;
//        case OBJECT_END: printf("}\n"); break;
//        case ARRAY_START: printf("[\n"); break;
//        case ARRAY_END: printf("]\n"); break;
//        case COMMA: printf(",\n"); break;
//        case COLON: printf(":\n"); break;
//        case STRING: printf("STRING: %s\n", token.value); break;
//        case NUMBER: printf("NUMBER: %s\n", token.value); break;
//        case TRUE: printf("TRUE\n"); break;
//        case FALSE: printf("FALSE\n"); break;
//        case NONE: printf("NONE\n"); break;
//        case ILLEGAL: printf("ILLEGAL\n"); break;
//        case END: break;
//        }
//    } while (token.kind != END);

    return 0;
}

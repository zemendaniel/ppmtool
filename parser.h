#pragma once
#include <stdio.h>

typedef enum {
    TOKEN_OK,
    TOKEN_EOF,
    TOKEN_TOO_LONG,
    TOKEN_INVALID_CAPACITY,
    TOKEN_FILE_ERROR
} TokenResult;


TokenResult nextToken(FILE *file, char *buffer, size_t capacity);
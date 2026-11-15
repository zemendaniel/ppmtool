#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "parser.h"


TokenResult nextToken(FILE *file, char *buffer, const int capacity) {
    if (capacity <= 0) {
        fprintf(stderr, "Capacity must be greater than 0");
        return TOKEN_INVALID_CAPACITY;
    }
    int bufferIndex = 0;
    while (true) {
        const int c = fgetc(file);
        
        if (c == EOF) {
            if (ferror(file)) return TOKEN_FILE_ERROR;

            if (bufferIndex == 0) return TOKEN_EOF;

            buffer[bufferIndex] = '\0';
            return TOKEN_OK;
        }
        
        if (isspace(c)) {
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';
                return TOKEN_OK;
            }
            continue;
        }
        
        if (c == '#') {
            while (true) {
                const int comment_c = fgetc(file);
                if (ferror(file)) return TOKEN_FILE_ERROR;
                if (comment_c == EOF || comment_c == '\n' || comment_c == '\r') break;
            }
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';
                return TOKEN_OK;
            }
            continue;
        }
        
        if (bufferIndex + 1 >= capacity) {
            fprintf(stderr, "The ppm file contains a token that is too large. It is more than %d characters.\n", capacity);
            return TOKEN_TOO_LONG;
        }
        
        buffer[bufferIndex++] = c;
    }    
}

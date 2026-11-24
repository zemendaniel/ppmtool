#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "parser.h"

/**
 * @brief Read the next token, skipping whitespace and comments beginning with a hash.
 * @param[in,out] file Open input stream. The caller is responsible for closing it.
 * @param[out] buffer Destination for the token. Use it only after @c TOKEN_OK.
 * @param[in] capacity Buffer size in bytes, including room for the terminating NUL.
 * @retval TOKEN_OK A token was stored and NUL-terminated.
 * @retval TOKEN_EOF The stream ended before another token was found.
 * @retval TOKEN_TOO_LONG The token does not fit in the buffer.
 * @retval TOKEN_INVALID_CAPACITY The capacity is zero.
 * @retval TOKEN_FILE_ERROR Reading the stream failed.
 */
TokenResult nextToken(FILE *file, char *buffer, size_t capacity) {
    if (capacity == 0) {
        fputs("Capacity must be greater than 0\n", stderr);
        return TOKEN_INVALID_CAPACITY;
    }
    size_t bufferIndex = 0;
    while (true) {
        // Read the next character
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
        
        if (bufferIndex == capacity - 1) {
            fprintf(stderr, "PPM token exceeds the maximum length of %zu characters.\n", capacity - 1);
            return TOKEN_TOO_LONG;
        }
        
        buffer[bufferIndex++] = (char)c;
    }    
}

#pragma once

typedef enum {
    TOKEN_OK,
    TOKEN_EOF,
    TOKEN_TOO_LONG,
    TOKEN_INVALID_CAPACITY,
    TOKEN_FILE_ERROR
} TokenResult;

/**
 * @brief Read the next token, skipping whitespace and comments beginning with a hash.
 * @param[in,out] file Open input stream. The caller is responsible for closing it.
 * @param[out] buffer Destination for the token. Use it only after @c TOKEN_OK.
 * @param[in] capacity Buffer size in bytes, including room for the terminating NUL.
 * @retval TOKEN_OK A token was stored and NUL-terminated.
 * @retval TOKEN_EOF The stream ended before another token was found.
 * @retval TOKEN_TOO_LONG The token does not fit in the buffer.
 * @retval TOKEN_INVALID_CAPACITY The capacity is zero or negative.
 * @retval TOKEN_FILE_ERROR Reading the stream failed.
 */
TokenResult nextToken(FILE *file, char *buffer, int capacity);
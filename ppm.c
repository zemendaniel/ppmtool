#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include "parser.h"
#include "ppm.h"
#include "debugmalloc.h"


/**
 * @brief Read the next token as a decimal integer that fits in an int.
 * @param[in,out] file Open input stream to read from.
 * @param[out] value Parsed integer, written on success.
 * @retval true The integer was read and stored.
 * @retval false Reading the token or converting it to an integer failed.
 */
static bool readInteger(FILE *file, int *value) {
    char buffer[32];

    if (nextToken(file, buffer, sizeof(buffer)) != TOKEN_OK) {
        return false;
    }

    char *end;
    errno = 0;

    const long number = strtol(buffer, &end, 10);

    if (errno == ERANGE ||
        end == buffer ||
        *end != '\0' ||
        number < INT_MIN ||
        number > INT_MAX) {
        return false;
        }

    *value = (int)number;
    return true;
}

/**
 * @brief Release an image, its row-pointer array, and its pixel buffer.
 * @param[in] image Image to release, or @c NULL.
 */
void freeImage(Image *image) {
    if (image == NULL) {
        return;
    }

    free(image->data);
    free(image->rows);
    free(image);
}

/**
 * @brief Report a load error and release resources acquired so far.
 * @param[in] file Stream to close, or @c NULL.
 * @param[in] image Partially loaded image to free, or @c NULL.
 * @param[in] errorMessage Diagnostic to write to stderr.
 * @return @c NULL to signal that loading failed.
 */
static Image* loadFailed(FILE *file, Image *image, const char *errorMessage) {
    fprintf(stderr, "Could not load PPM file %s\n", errorMessage);
    if (file != NULL) {
        fclose(file);
    }

    freeImage(image);
    return NULL;
}

/**
 * @brief Load a P3 image with a Maxval of 255.
 * @param[in] filename Path to the image file.
 * @return The loaded image, which the caller must release with freeImage(), or @c NULL on a recoverable error.
 */
Image *loadImage(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return NULL;
    }

    Image *image = NULL;

    char header[3];
    if (nextToken(file, header, sizeof(header)) != TOKEN_OK || strcmp(header, "P3") != 0) {
        return loadFailed(file, image, "Invalid PPM header");
    }

    int width, height, maxValue;

    if (!readInteger(file, &width) ||
        !readInteger(file, &height) ||
        !readInteger(file, &maxValue)) {
        return loadFailed(file, image, "Invalid dimensions or Maxval");
    }

    if (width <= 0 || height <= 0) {
        return loadFailed(file, image, "Dimensions out of range");
    }
    if (maxValue != 255) {
        return loadFailed(file, image, "Unsupported Maxval (expected 255)");
    }

    const size_t w = (size_t)width;
    const size_t h = (size_t)height;

        if (w > SIZE_MAX / h) {
        return loadFailed(file, image, "Image dimensions overflow");
    }

    const size_t pixelCount = w * h;

    if (pixelCount > SIZE_MAX / sizeof(Pixel) ||
        h > SIZE_MAX / sizeof(Pixel*)) {
        return loadFailed(file, image, "Image allocation size overflow");
    }

    image = malloc(sizeof(*image));

    if (image == NULL) {
        return loadFailed(file, image, "Image allocation failed");
    }

    image->width = width;
    image->height = height;
    image->maxValue = maxValue;
    image->rows = NULL;
    image->data = NULL;

    image->rows = malloc(h * sizeof(*image->rows));

    if (image->rows == NULL) {
        return loadFailed(file, image, "Row allocation failed");
    }

    image->data = malloc(pixelCount * sizeof(*image->data));

    if (image->data == NULL) {
        return loadFailed(file, image, "Pixel allocation failed");
    }

    for (size_t y = 0; y < h; y++) {
        image->rows[y] = image->data + y * w;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r, g, b;

            if (!readInteger(file, &r) ||
                !readInteger(file, &g) ||
                !readInteger(file, &b)) {
                return loadFailed(file, image, "Incomplete pixel data");
            }

            if (r < 0 || r > maxValue ||
                g < 0 || g > maxValue ||
                b < 0 || b > maxValue) {
                return loadFailed(file, image, "Invalid RGB value");
            }

            image->rows[y][x] = (Pixel) {
                .r = (unsigned char)r,
                .g = (unsigned char)g,
                .b = (unsigned char)b
            };
        }
    }

    char extra[2];
    if (nextToken(file, extra, sizeof(extra)) != TOKEN_EOF) {
        return loadFailed(file, image, "Unexpected data after pixels");
    }

    if (fclose(file) != 0) {
        freeImage(image);
        fprintf(stderr, "Could not close PPM file\n");
        return NULL;
    }

    return image;
}

/**
 * @brief Write an image as P3, replacing the destination's existing contents.
 * @param[in] image Image to write. The caller retains ownership.
 * @param[in] filename Path to the output file.
 * @retval true The file was written and closed successfully.
 * @retval false A file operation failed. The output file may be partial.
 */
bool writeImage(const Image *image, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return false;
    }
    
    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n", image->width, image->height);
    fprintf(file, "%d\n", image->maxValue);
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            const Pixel *pixel = &image->rows[y][x];
            fprintf(file, "%d %d %d\n", pixel->r, pixel->g, pixel->b);
        }
    }
    bool success = !ferror(file);
    if (fclose(file) == EOF) {
        success = false;
    }

    return success;
}
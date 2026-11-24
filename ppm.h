#pragma once
#include <stdbool.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

/* Image owns both arrays. Each entry in rows points to the start of a row in data. */
typedef struct {
    int width;
    int height;
    int maxValue;
    Pixel **rows;
    Pixel *data;
} Image; 

/**
 * @brief Load a P3 image with a Maxval in the range 1 through 255.
 * @param[in] filename Path to the image file.
 * @return The loaded image, which the caller must release with freeImage(),
 *         or @c NULL on a recoverable error.
 */
Image *loadImage(const char *filename);

/**
 * @brief Release an image, its row-pointer array, and its pixel buffer.
 * @param[in] image Image to release, or @c NULL.
 */
void freeImage(Image *image);

/**
 * @brief Write an image as P3, replacing the destination's existing contents.
 * @param[in] image Image to write. The caller retains ownership.
 * @param[in] filename Path to the output file.
 * @retval true The file was written and closed successfully.
 * @retval false A file operation failed. The output file may be partial.
 */
bool writeImage(const Image *image, const char *filename);
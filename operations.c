#include "operations.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugmalloc.h"

/**
 * @brief Calculate the allocation size of a contiguous pixel buffer.
 * @param[in] width Number of pixels per row.
 * @param[in] height Number of rows.
 * @param[out] bytes Allocation size, written on success.
 * @retval true The allocation size fits in size_t.
 * @retval false A dimension is zero or the allocation size would overflow.
 */
static bool pixelBufferSize(size_t width, size_t height, size_t *bytes) {
    // Prevent integer overflow
    if (width == 0 || height == 0 || width > SIZE_MAX / height / sizeof(Pixel)) {
        return false;
    }

    *bytes = width * height * sizeof(Pixel);
    return true;
}

/**
 * @brief Replace each pixel's channels with their average.
 * @param[in,out] image Image to convert to grayscale in place.
 */
void applyGrayscale(Image *image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            Pixel *pixel = &image->rows[i][j];
            const unsigned char gray = (unsigned char)((pixel->r + pixel->g + pixel->b) / 3);
            pixel->r = gray;
            pixel->g = gray;
            pixel->b = gray;
        }
    }
}

/**
 * @brief Invert the colors by subtracting each channel from 255.
 * @param[in,out] image Image to invert in place.
 */
void applyNegative(Image *image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            Pixel *pixel = &image->rows[i][j];
            pixel->r = 255 - pixel->r;
            pixel->g = 255 - pixel->g;
            pixel->b = 255 - pixel->b;
        }
    }
}

/**
 * @brief Clamp a color channel value to the range 0 through 255.
 * @param[in] value Channel value before clamping.
 * @return The clamped channel value.
 */
static unsigned char clamp(int value) {
    if (value < 0) 
        return 0;
    if (value > 255) 
        return 255;
    return (unsigned char)value;
}

/**
 * @brief Add an offset to each color channel and clamp the result to 0 through 255.
 * @param[in,out] image Image to adjust in place.
 * @param[in] offset Signed brightness adjustment in the range -100 through 100.
 */
void applyBrightness(Image *image, int offset) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            Pixel *pixel = &image->rows[i][j];
            pixel->r = clamp(pixel->r + offset);
            pixel->g = clamp(pixel->g + offset);
            pixel->b = clamp(pixel->b + offset);
        }
    }
}

/**
 * @brief Replace each interior pixel with the average of its original 3x3 neighborhood.
 * @param[in,out] image Image to blur in place, preserving the border.
 * @retval true The blur completed.
 * @retval false The temp buffer size was invalid or allocation failed. 
 */
bool applyBlur(Image *image) {
    const size_t width = (size_t)image->width;
    const size_t height = (size_t)image->height;
    size_t pixelBytes;
    if (!pixelBufferSize(width, height, &pixelBytes)) {
        return false;
    }

    Pixel *oldData = malloc(pixelBytes);
    if (oldData == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for blur operation.\n");
        return false;
    }
    memcpy(oldData, image->data, pixelBytes);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // Skip the border pixels
            if (x == 0 || y == 0 || x == image->width - 1 || y == image->height - 1)
                continue;
            
            int r = 0, g = 0, b = 0;
            int count = 0;
            
            for (int offsetY = -1; offsetY <= 1; offsetY++)
                for (int offsetX = -1; offsetX <= 1; offsetX++) {
                    const int neighborY = y + offsetY;
                    const int neighborX = x + offsetX;
                    
                    if (neighborY < 0 || neighborX < 0 || neighborY >= image->height || neighborX >= image->width) {
                        continue;
                    }
                    const size_t index = (size_t)neighborY * width + (size_t)neighborX;
                    const Pixel pixel = oldData[index];

                    r += pixel.r;
                    g += pixel.g;
                    b += pixel.b;
                    count++;
                }
            Pixel *currentPixel = &(image->rows[y][x]);
            currentPixel->r = (unsigned char)(r / count);
            currentPixel->g = (unsigned char)(g / count);
            currentPixel->b = (unsigned char)(b / count);
        }
    }
    free(oldData);
    return true;
}

/**
 * @brief Flip an image horizontally (the pixels swap horizontally, so left-right mirror) by reversing the pixels in each row.
 * @param[in,out] image Image to flip in place.
 */
void applyMirror(Image *image) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width / 2; x++) {
            Pixel *leftPixel = &(image->rows[y][x]);
            Pixel *rightPixel = &(image->rows[y][image->width - 1 - x]);
            Pixel temp = *leftPixel;
            *leftPixel = *rightPixel;
            *rightPixel = temp;
        }
    }
}

/**
 * @brief Rotate an image 90 degrees clockwise.
 * @param[in,out] image Image whose dimensions and pixel storage will be updated.
 * @retval true Rotation succeeded, swapping dimensions and replacing pixel storage.
 * @retval false The storage size was invalid or allocation failed. 
 */
bool applyRotate(Image *image) {
    const int oldHeight = image->height;
    const int oldWidth = image->width;
    const size_t newWidth = (size_t)oldHeight;
    const size_t newHeight = (size_t)oldWidth;
    size_t pixelBytes;
    if (!pixelBufferSize(newWidth, newHeight, &pixelBytes) ||
        newHeight > SIZE_MAX / sizeof(Pixel *)) {
        return false;
    }

    Pixel *newData = malloc(pixelBytes);
    Pixel **newRows = malloc(newHeight * sizeof(*newRows));
    if (newData == NULL || newRows == NULL) {
        free(newData);
        free(newRows);
        return false;
    }
    
    for (size_t y = 0; y < newHeight; y++) {
        newRows[y] = newData + y * newWidth;
    }
    
    for (int y = 0; y < oldHeight; y++) {
        for (int x = 0; x < oldWidth; x++) {
            newRows[x][oldHeight - 1 - y] = image->rows[y][x];
        }
    }
    
    free(image->data);
    free(image->rows);

    image->rows = newRows;
    image->data = newData;
    image->width = oldHeight;
    image->height = oldWidth;
    return true;
}

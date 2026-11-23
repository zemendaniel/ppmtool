#include "debugmalloc.h"
#include "ppm.h"

void applyGrayscale(Image *image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            Pixel *pixel = &image->rows[i][j];
            const int gray = (pixel->r + pixel->g + pixel->b) / 3;
            pixel->r = gray;
            pixel->g = gray;
            pixel->b = gray;
        }
    }
}

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

static unsigned char clamp(const int value) {
    if (value < 0) 
        return 0;
    if (value > 255) 
        return 255;
    return value;
}

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

void applyBlur(Image *image) {
    Pixel *oldData = malloc(image->width * image->height * sizeof(Pixel));
    Pixel **oldRows = malloc(image->height * sizeof(Pixel*));
    if (oldData == NULL || oldRows == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for blur operation.\n");
        return;
    }
    const size_t size = (size_t)image->width * image->height;
    memcpy(oldData, image->rows[0],  size * sizeof(Pixel));
    
    for (int y = 0; y < image->height; y++) {
        oldRows[y] = oldData + y * image->width;
    }
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int r = 0, g = 0, b = 0;
            int count = 0;
            
            for (int offsetY = -1; offsetY <= 1; offsetY++)
                for (int offsetX = -1; offsetX <= 1; offsetX++) {
                    int neighborY = y + offsetY;
                    int neighborX = x + offsetX;
                    
                    if (neighborY < 0 || neighborX < 0 || neighborY >= image->height || neighborX >= image->width) {
                        continue;
                    }
                    const Pixel pixel = oldRows[neighborY][neighborX];

                    r += pixel.r;
                    g += pixel.g;
                    b += pixel.b;
                    count++;
                }
            Pixel *currentPixel = &(image->rows[y][x]);
            currentPixel->r = r / count;
            currentPixel->g = g / count;
            currentPixel->b = b / count;
        }
    }
    free(oldData);
    free(oldRows);
}

void applyMirror(Image *image) {
    
}

void applyRotate(Image *image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            
        }
    }
}

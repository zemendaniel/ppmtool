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
    
}

void applyMirror(Image *image) {
    
}

void applyRotate(Image *image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            
        }
    }
}

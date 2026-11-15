#pragma once
#include <stdbool.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct {
    int width;
    int height;
    int maxValue;
    Pixel **rows;
    Pixel *data;
} Image; 

Image *loadImage(const char *filename);

void freeImage(Image *image);

bool writeImage(const Image *image, const char *filename);
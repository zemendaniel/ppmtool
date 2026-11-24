#pragma once
#include "ppm.h"

void applyGrayscale(Image *image);
bool applyBlur(Image *image);
void applyNegative(Image *image);
void applyBrightness(Image *image, int offset);
void applyMirror(Image *image);
bool applyRotate(Image *image);
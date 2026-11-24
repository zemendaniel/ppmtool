#pragma once
#include "ppm.h"

/**
 * @brief Replace each pixel's channels with their integer mean.
 * @param[in,out] image Image to convert to grayscale in place.
 */
void applyGrayscale(Image *image);

/**
 * @brief Replace each interior pixel with the average of its original 3x3 neighborhood.
 * @param[in,out] image Image to blur in place, preserving the border.
 * @retval true The blur completed.
 * @retval false Temporary allocation failed before any pixels were changed.
 */
bool applyBlur(Image *image);

/**
 * @brief Invert the colors by subtracting each channel from 255.
 * @param[in,out] image Image to invert in place.
 */
void applyNegative(Image *image);

/**
 * @brief Add an offset to each color channel and clamp the result to 0 through 255.
 * @param[in,out] image Image to adjust in place.
 * @param[in] offset Signed brightness adjustment in the range -100 through 100.
 */
void applyBrightness(Image *image, int offset);

/**
 * @brief Flip an image horizontally by reversing the pixels in each row.
 * @param[in,out] image Image to flip in place.
 */
void applyMirror(Image *image);

/**
 * @brief Rotate an image 90 degrees clockwise.
 * @param[in,out] image Image whose dimensions and pixel storage will be updated.
 * @retval true Rotation succeeded, swapping dimensions and replacing pixel storage.
 * @retval false Allocation failed and the original image was preserved.
 */
bool applyRotate(Image *image);
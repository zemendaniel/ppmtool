#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"
#include "operations.h"
#include "debugmalloc.h"


int main(const int argc, char *argv[]) {
    const char helpString[] = "Usage: ppmtool <input.ppm> <output.ppm> <operation>";
    
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        printf("%s\n", helpString);
        // "Available operations:\n"); // todo docs for operations
        return 0;
    }
    if (argc < 4) {
        printf("%s\n", helpString);
        return 1;
    }
    
    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];
    const char *operation = argv[3];
    
    Image *image = loadImage(inputFilename);
    if (image == NULL) {
        printf("Could not load image %s\n", inputFilename);
        return 1;
    }
    
    if (strcmp(operation, "brightness") == 0) {
        if (argc < 5) {
            printf("Error: brightness operation requires a value\n");
            freeImage(image);
            return 1;
        }
        char *end;
        errno = 0;
        const long value = strtol(argv[4], &end, 10);
        if (end == argv[4] || *end != '\0') {
            printf("Error: brightness value must be an integer\n");
            freeImage(image);
            return 1;
        }
        if (errno == ERANGE || value < -100 || value > 100) {
            printf("Error: brightness value must be between -100 and 100\n");
            freeImage(image);
            return 1;
        }
        const int brightnessValue = (int)value;
        applyBrightness(image, brightnessValue);
    }
    else if (strcmp(operation,  "grayscale") == 0) 
        applyGrayscale(image);
    
    else if (strcmp(operation,  "negative") == 0) 
        applyNegative(image);
    
    else if (strcmp(operation,  "mirror") == 0) {
        
    }
    else if (strcmp(operation,  "rotate90") == 0) {
        
    }
    else if (strcmp(operation,  "blur") == 0) {
        applyBlur(image);
    }
    else {
        printf("Error: unknown operation %s\n", operation);
        return 1;
    }
    
    const bool writeSuccess = writeImage(image, outputFilename);
    freeImage(image);
    if (!writeSuccess) {
        printf("Error writing image to %s\n", outputFilename);
        return 1;
    }
    
    return 0;
}

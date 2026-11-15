#include <stdio.h>
#include <string.h>
#include "ppm.h"
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
    
    // todo operation = brightness then value needed too
    
    Image *image = loadImage(inputFilename);
    if (image == NULL) {
        printf("Could not load image %s\n", inputFilename);
        return 1;
    }
    
    // todo switch statement for the operations
    
    const bool writeSuccess = writeImage(image, outputFilename);
    freeImage(image);
    if (!writeSuccess) {
        printf("Error writing image to %s\n", outputFilename);
        return 1;
    }
    
    return 0;
}

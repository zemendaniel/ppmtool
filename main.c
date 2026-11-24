#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "operations.h"

typedef enum {
    GRAYSCALE,
    NEGATIVE,
    BRIGHTNESS,
    MIRROR,
    ROTATE90,
    BLUR,
    OPERATION_COUNT
} Operation;

static const struct {
    const char *name;
    const char *description;
} operations[OPERATION_COUNT] = {
    [GRAYSCALE] = {"grayscale", "Convert the image to grayscale."},
    [NEGATIVE] = {"negative", "Invert the colors."},
    [BRIGHTNESS] = {"brightness", "Adjust brightness by an integer N from -100 to 100."},
    [MIRROR] = {"mirror", "Flip the image horizontally."},
    [ROTATE90] = {"rotate90", "Rotate the image 90 degrees clockwise."},
    [BLUR] = {"blur", "Apply a 3x3 box blur and preserve the border pixels."}
};

static void printHelp(void) {
    puts("Usage: ppmtool <input.ppm> <output.ppm> <operation> [N]\n"
         "       ppmtool --help\n\n"
         "Process a P3 PPM image with one operation.\n\n"
         "Operations:");

    for (Operation operation = GRAYSCALE; operation < OPERATION_COUNT; operation++) {
        printf("  %s%s\n      %s\n", operations[operation].name,
               operation == BRIGHTNESS ? " N" : "", operations[operation].description);
    }
}

static Operation findOperation(const char *name) {
    for (Operation operation = GRAYSCALE; operation < OPERATION_COUNT; operation++) {
        if (strcmp(name, operations[operation].name) == 0) {
            return operation;
        }
    }
    return OPERATION_COUNT;
}

static bool parseBrightness(const char *argument, int *offset) {
    char *end;
    errno = 0;
    const long value = strtol(argument, &end, 10);
    if (end == argument || *end != '\0') {
        fputs("Error: brightness value must be an integer\n", stderr);
        return false;
    }
    if (errno == ERANGE || value < -100 || value > 100) {
        fputs("Error: brightness value must be between -100 and 100\n", stderr);
        return false;
    }

    *offset = (int)value;
    return true;
}

static bool applyOperation(Image *image, Operation operation, int brightnessOffset) {
    switch (operation) {
        case GRAYSCALE:
            applyGrayscale(image);
            return true;
        case NEGATIVE:
            applyNegative(image);
            return true;
        case BRIGHTNESS:
            applyBrightness(image, brightnessOffset);
            return true;
        case MIRROR:
            applyMirror(image);
            return true;
        case ROTATE90:
            return applyRotate(image);
        case BLUR:
            return applyBlur(image);
        case OPERATION_COUNT:
            return false;
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        if (argc != 2) {
            fputs("Error: --help takes no arguments\n", stderr);
            return 1;
        }
        printHelp();
        return 0;
    }

    for (int i = 1; i <= 2 && i < argc; i++) {
        if (argv[i][0] == '-') {
            fprintf(stderr, "Error: unexpected option %s (use ./ for filenames beginning with -)\n",
                    argv[i]);
            return 1;
        }
    }
    if (argc < 4) {
        fputs("Error: expected an input file, an output file, and an operation\n"
              "Run ppmtool --help for usage.\n", stderr);
        return 1;
    }

    const Operation operation = findOperation(argv[3]);
    if (operation == OPERATION_COUNT) {
        fprintf(stderr, "Error: unknown operation %s\n", argv[3]);
        return 1;
    }

    const int expectedArgc = operation == BRIGHTNESS ? 5 : 4;
    if (argc > expectedArgc) {
        fprintf(stderr, "Error: too many arguments for %s\n", operations[operation].name);
        return 1;
    }
    if (argc < expectedArgc) {
        fputs("Error: brightness operation requires a value\n", stderr);
        return 1;
    }

    int brightnessOffset = 0;
    if (operation == BRIGHTNESS && !parseBrightness(argv[4], &brightnessOffset)) {
        return 1;
    }

    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];
    Image *image = loadImage(inputFilename);
    if (image == NULL) {
        return 1;
    }

    if (!applyOperation(image, operation, brightnessOffset)) {
        fprintf(stderr, "Error applying %s operation\n", operations[operation].name);
        freeImage(image);
        return 1;
    }

    const bool writeSuccess = writeImage(image, outputFilename);
    freeImage(image);
    if (!writeSuccess) {
        fprintf(stderr, "Error writing image to %s\n", outputFilename);
        return 1;
    }

    return 0;
}

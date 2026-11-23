What we have right now:
 - load and store a PPM P3 (text format) image in memory
    - ignore comments and extra whitespace
    - check the header, dimensions and maximum colour value (1–255)
    - validate RGB values and reject missing or extra pixel data
    - check allocation sizes and clean up if loading or writing fails
 - write a P3 image to disk and report write errors
 - show basic command line usage with `--help`
 - accept `ppmtool <input.ppm> <output.ppm> <operation>`; currently this just copies the image, since the operation is not used yet

What still needs to be done:
 - implement image operations and select them from the command line
 - read and validate extra arguments, such as a brightness value
 - document the available operations in the help message
 - add automated checks for loading, writing and invalid input

What the files and functions do:
 - `main.c`: `main()` reads arguments, loads the image, writes the output and frees memory after a successful write
 - `ppm.h`: defines `Pixel` (RGB channels) and `Image` (dimensions, maximum colour value, pixel data and row pointers), and declares the image functions
 - `ppm.c`: handles image storage and file input/output
    - `loadImage()` reads and validates a P3 file, then allocates and fills the image
    - `writeImage()` saves the image as P3; `freeImage()` releases its memory
    - `readInteger()` converts a token to an integer with range checks; `loadFailed()` reports loading errors and cleans up
 - `parser.h` / `parser.c`: declare token result codes and implement `nextToken()`, which reads tokens while skipping whitespace and comments, and reports EOF, file errors or oversized tokens
 - `CMakeLists.txt`: configures the C (C99) executable and its source files
 - `cica.ppm`: sample P3 image for trying the program

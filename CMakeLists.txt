cmake_minimum_required(VERSION 4.3)
project(ppmtool C)

set(CMAKE_C_STANDARD 99)

add_executable(ppmtool main.c
        ppm.c
        parser.c
        parser.c
        parser.h
        ppm.h
        operations.c
        operations.h)

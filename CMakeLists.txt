cmake_minimum_required(VERSION 3.27)
project(JPEG_Analysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(JPEG_Analysis main.cpp
        imageData.h
        RGBtoY.h
        additiveNoise.h
        impulseNoise.h)

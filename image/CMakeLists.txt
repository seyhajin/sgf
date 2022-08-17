cmake_minimum_required(VERSION 3.8)

##### image module #####

project(image)

add_library(image
        image.cpp
        image.hh
        image.h
        imageloader.cpp
        imageloader.h
        imageutils.cpp
        imageutils.h
        )

target_link_libraries(image geom stb)

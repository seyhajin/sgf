cmake_minimum_required(VERSION 3.8)

project(image)

add_library(image
        image.cpp
        image.hh
        image.h
        imageutils.cpp
        imageutils.h
        imageloader.cpp imageloader.h)

target_link_libraries(image imgui stb core)

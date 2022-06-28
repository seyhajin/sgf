cmake_minimum_required(VERSION 3.8)

##### stb module #####

project(stb)

add_library(stb
        stb.cpp
        stb.hh
        )

#target_include_directories(stb PUBLIC ${PROJECT_SOURCE_DIR}/libstb)

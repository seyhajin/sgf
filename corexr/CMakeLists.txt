
cmake_minimum_required(VERSION 3.8)

##### core3d module #####

project(corexr)

add_library(corexr
        corexr.cpp
        corexr.hh
        openxrsystem.cpp
        openxrsystem.h
        webxrsystem.cpp
        webxrsystem.h
        xrsystem.cpp
        xrsystem.h)

target_link_libraries(corexr core3d)

##### corexr tests #####

add_executable(hellotrianglexr tests/hellotrianglexr.cpp)
target_link_libraries(hellotrianglexr corexr)

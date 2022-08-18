cmake_minimum_required(VERSION 3.8)

##### geom module #####

project(geom)

add_library(geom
        affinemat4.cpp
        affinemat4.h
        box.cpp
        box.h
        geom.cpp
        geom.hh
        line.cpp
        line.h
        mat3.cpp
        mat3.h
        mat4.cpp
        mat4.h
        plane.cpp
        plane.h
        quat.cpp
        quat.h
        rect.cpp
        rect.h
        vec2.cpp
        vec2.h
        vec3.cpp
        vec3.h
        vec4.cpp
        vec4.h
        )

target_link_libraries(geom core)

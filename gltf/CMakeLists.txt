cmake_minimum_required(VERSION 3.8)

##### gltf module #####

project(gltf)

add_library(gltf
		gltf.cpp
		gltf.h
		gltf.hh)

target_include_directories(gltf PRIVATE ${PROJECT_SOURCE_DIR}/libtinygltf)

target_link_libraries(gltf scene stb)

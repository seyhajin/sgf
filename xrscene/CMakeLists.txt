cmake_minimum_required(VERSION 3.8)

if(SGF_GITHUB_BUILD)
	return()
endif()

##### xrscene library #####

project(xrscene)

add_library(xrscene
	xrcamera.cpp
	xrcamera.h
	xrscene.hh
)

target_link_libraries(xrscene webxr scene)

##### scene tests #####

add_executable(helloboxxr tests/helloboxxr.cpp )

target_link_libraries(helloboxxr xrscene)
target_copy_assets(helloboxxr scene)
target_link_assets(helloboxxr)

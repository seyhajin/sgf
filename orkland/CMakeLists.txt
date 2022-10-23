cmake_minimum_required(VERSION 3.8)

##### orkland module #####

project(orkland)

add_executable(orkland
        orkland.cpp
        orkland.h
        texturestreamer.cpp
        texturestreamer.h
        level.cpp
		level.h
		airplane.cpp
		airplane.h
		)

target_link_libraries(orkland scene fetch)

target_copy_assets(orkland orkland)
target_copy_assets(orkland scene)

target_link_assets(orkland)

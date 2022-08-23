cmake_minimum_required(VERSION 3.8)

##### gltf test saucertest #####

add_executable(saucertest saucertest.cpp)

target_link_libraries(saucertest scene gltf)
target_copy_assets(saucertest saucertest)
target_copy_assets(saucertest scene)
target_link_assets(saucertest)

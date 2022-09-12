cmake_minimum_required(VERSION 3.8)

##### core tests #####

if(NOT OS_EMSCRIPTEN)
    add_executable(promisetest promisetest.cpp)
    target_link_libraries(promisetest core)
endif()

add_executable(coretests coretests.cpp)
target_link_libraries(coretests core)

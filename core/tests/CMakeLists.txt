cmake_minimum_required(VERSION 3.8)

##### core tests #####

project(coretests)

add_executable(promisetest promisetest.cpp)
target_link_libraries(promisetest core)
if(OS_EMSCRIPTEN)
    target_link_options(promisetest PRIVATE "SHELL:-pthread -sPTHREAD_POOL_SIZE=4")
#    target_link_options(promisetest PRIVATE "SHELL:-pthread -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency")
endif()

add_executable(coretests coretests.cpp)
target_link_libraries(coretests core)

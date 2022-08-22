
cmake_minimum_required(VERSION 3.8)

if (OS_EMSCRIPTEN)
    return()
endif ()

##### emgdb utility #####

add_executable(emgdb emgdb.cpp)

target_link_libraries(emgdb)# core)


cmake_minimum_required(VERSION 3.8)

if(NOT OS_EMSCRIPTEN AND NOT OS_WINDOWS)
    return()
endif()

##### corexr module #####

project(corexr)

set(SOURCES corexr.cpp corexr.hh)

if(OS_EMSCRIPTEN)
    set(SOURCES ${SOURCES} webxrsystem.cpp webxrsystem.h)
elseif(OS_WINDOWS)
    set(SOURCES ${SOURCES} openxrsystem.cpp openxrsystem.h)
endif()

add_library(corexr ${SOURCES})

if(OS_EMSCRIPTEN)

    target_link_libraries(corexr PUBLIC core3d)

elseif (OS_WINDOWS)

    target_include_directories(corexr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/include"
            )

    target_link_libraries(corexr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/windows_x64/lib/openxr_loader.lib"
            core3d
            )
endif()

##### corexr tests #####

add_executable(hellotrianglexr tests/hellotrianglexr.cpp)
target_link_libraries(hellotrianglexr corexr)

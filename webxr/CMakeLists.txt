
cmake_minimum_required(VERSION 3.8)

##### webxr module #####

project(webxr)

set(SOURCES webxr.hh)

if (OS_EMSCRIPTEN)
    set(SOURCES ${SOURCES} webxrsystem.cpp webxrsystem.h)
else ()
    set(SOURCES ${SOURCES} openxrsystem.cpp openxrsystem.h)
endif ()

add_library(webxr ${SOURCES})

if (OS_EMSCRIPTEN)

    target_link_libraries(webxr PUBLIC core3d)

elseif (OS_WINDOWS)

    target_include_directories(webxr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/include"
            )

    target_link_libraries(webxr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/windows_x64/lib/openxr_loader.lib"
            core3d
            )
else ()

    find_package(OpenXR REQUIRED)

    target_include_directories(webxr PUBLIC OpenXR_INCLUDE_DIRS)

    target_link_libraries(webxr PUBLIC OpenXR_LIBRARIES core3d)

endif ()

##### webxr tests #####

add_executable(hellotrianglexr tests/hellotrianglexr.cpp)

target_link_libraries(hellotrianglexr webxr)

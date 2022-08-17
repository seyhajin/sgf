cmake_minimum_required(VERSION 3.8)

if (SGF_GITHUB_BUILD)
    return()
endif ()

##### webxr module #####

project(webxr)

set(SOURCES xrsystem.cpp xrsystem.h webxr.hh)

if (OS_EMSCRIPTEN)
    set(SOURCES ${SOURCES} webxrsystem.cpp webxrsystem.h)
else ()
    set(SOURCES ${SOURCES} openxrsystem.cpp openxrsystem.h)
endif ()

add_library(webxr ${SOURCES})

if (OS_EMSCRIPTEN)
    target_link_libraries(webxr core3d)
elseif (OS_WINDOWS)
    target_include_directories(webxr PRIVATE "${PROJECT_SOURCE_DIR}/libopenxr/include")
    target_link_libraries(webxr core3d "${PROJECT_SOURCE_DIR}/libopenxr/windows_x64/lib/openxr_loader.lib")
else ()
    find_package(OpenXR REQUIRED)
    target_include_directories(webxr PRIVATE "${OpenXR_INCLUDE_DIRS}")
    target_link_libraries(webxr core3d "${OpenXR_LIBRARIES}")
endif ()

##### webxr tests #####

add_executable(hellotrianglexr tests/hellotrianglexr.cpp)
target_link_libraries(hellotrianglexr webxr)

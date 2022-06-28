# To configure for WebAssembly in a build subdir:
#
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
#
# Note: Env var EMSCRIPTEN must be set to something like emsdk/upstream/emscripten or root dir of github emscripten checkout.

# To build in build subdir:
#
# cmake --build .

cmake_minimum_required(VERSION 3.8)

project(blitz-world)

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    set(OS_EMSCRIPTEN 1)
    add_compile_definitions(OS_EMSCRIPTEN=1)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(OS_WINDOWS 1)
    add_compile_definitions(OS_WINDOWS=1)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(OS_LINUX 1)
    add_compile_definitions(OS_LINUX=1)
    #elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    #    set(DARWIN 1)
else ()
    message(FATAL_ERROR "!!! Unrecognized CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBUG 1)
else ()
    set(RELEASE 1)
endif ()

if (OS_EMSCRIPTEN)
    set(USE_OPENGLES 1)
    add_compile_definitions(USE_OPENGLES=1)
endif ()

message("### Configuring cmake for CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} and CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} USE_OPENGLES=${USE_OPENGLES}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)

# Disable exceptions and RTTI
#
if (OS_WINDOWS)
    add_compile_options(/GR- /EHs- /EHc- /MD /D_HAS_EXCEPTIONS=0)
else ()
    add_compile_options(-fno-rtti -fno-exceptions)
    add_link_options(-fno-rtti -fno-exceptions)
endif ()

# Release optimization levels
#
if (RELEASE)
    if (OS_EMSCRIPTEN)
        add_compile_options(-Os)
        add_link_options(-Os)
    elseif (OS_LINUX)
        add_compile_options(-O3)
    endif ()
endif ()

# Enable sanitizers for linux debug builds
#
if (OS_LINUX AND DEBUG)
    set(SANITIZER_OPTIONS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    add_compile_options(${SANITIZER_OPTIONS})
    add_link_options(${SANITIZER_OPTIONS})
    message("### SANITIZER_OPTIONS=${SANITIZER_OPTIONS}")
endif ()

# Output html for emscripten builds
#
if (OS_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(--shell-file "${CMAKE_SOURCE_DIR}/embuild/index_template.html")
endif ()

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(core3d)
add_subdirectory(geom)
add_subdirectory(glfw)
add_subdirectory(glwindow)
add_subdirectory(image)
add_subdirectory(imgui)
add_subdirectory(opengl)
add_subdirectory(openxr)
add_subdirectory(scene)
add_subdirectory(stb)

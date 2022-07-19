# To configure for WebAssembly in a build subdir:
#
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
#
# Note: Env var EMSCRIPTEN must be set to something like emsdk/upstream/emscripten or root dir of github emscripten checkout.

# To build in build subdir:
#
# cmake --build .

cmake_minimum_required(VERSION 3.8)

project("Simple Game Framework")

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebugInfo)
endif ()

message("### Configuring project for:")
message("### CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("### CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("### CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")

# Build type
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG 1)
    add_compile_definitions(DEBUG=1)
else ()
    set(RELEASE 1)
    add_compile_definitions(RELEASE=1)
endif ()

# System name
#
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(OS_EMSCRIPTEN 1)
    add_compile_definitions(OS_EMSCRIPTEN=1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_WINDOWS 1)
    add_compile_definitions(OS_WINDOWS=1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_LINUX 1)
    add_compile_definitions(OS_LINUX=1)
else()
    message(FATAL_ERROR "!!! Unrecognized CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
endif ()

# Compiler
#
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_CLANG 1)
    add_compile_definitions(COMPILER_CLANG=1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_GCC 1)
    add_compile_definitions(COMPILER_GCC=1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILER_MSVC 1)
    add_compile_definitions(COMPILER_MSVC=1)
endif()

if (OS_EMSCRIPTEN)
    set(USE_OPENGLES 1)
    add_compile_definitions(USE_OPENGLES=1)
endif ()

# Add our cmake 'find' scripts
#
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# C++17 FTW!
#
set(CMAKE_CXX_STANDARD 17)

# Disable exceptions and RTTI
#
if (COMPILER_MSVC)
    string(REPLACE "/GR /EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    add_compile_options(/GR- /EHsc- /MD /D_HAS_EXCEPTIONS=0)
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

# TODO: Get memory sanitizer working on other systems:
#
if (OS_LINUX AND DEBUG)
    set(SANITIZER_OPTIONS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    add_compile_options(${SANITIZER_OPTIONS})
    add_link_options(${SANITIZER_OPTIONS})
    add_compile_definitions(ASAN_ENABLED=1)
    message("### Address sanitizer enabled: ${SANITIZER_OPTIONS}")
endif ()

# Output html for emscripten builds
#
if (OS_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(--shell-file "${CMAKE_SOURCE_DIR}/embuild/index_template.html")
    add_link_options("SHELL:--emrun -s TOTAL_MEMORY=8mb -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS=_main,_free")
endif ()

function(target_copy_assets target source)
    add_custom_command(TARGET ${target} PRE_LINK COMMAND ${CMAKE_COMMAND} -E copy_directory
    "$<TARGET_PROPERTY:${source},SOURCE_DIR>/assets" "$<TARGET_PROPERTY:${target},BINARY_DIR>/assets")
endfunction()

function(target_link_assets target)
    if(OS_EMSCRIPTEN)
        target_link_options(${target} PRIVATE --preload-file "$<TARGET_PROPERTY:${target},BINARY_DIR>/assets@/")
    endif()
endfunction()

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(core3d)
add_subdirectory(fetch)
add_subdirectory(geom)
add_subdirectory(glfw)
add_subdirectory(glwindow)
add_subdirectory(image)
add_subdirectory(imgui)
add_subdirectory(opengl)
add_subdirectory(openxr)
add_subdirectory(scene)
add_subdirectory(stb)

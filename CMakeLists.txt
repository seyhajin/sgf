# To configure for WebAssembly in a build subdir:
#
# cmake -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
#
# Note: Env var EMSCRIPTEN must be set to something like emsdk/upstream/emscripten or root dir of github emscripten checkout.
#
# To build in build subdir:
#
# cmake --build .
#
# TODO: This is probably getting to be too bit, some of it should be moved to core/CMakeLists.txt
#
# TODO: What should this really be?
cmake_minimum_required(VERSION 3.8)

project("Simple Game Framework")

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebugInfo)
endif ()

message("### Configuring Simple Game Framework for:")
message("### CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("### CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("### CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
#message("### SGF_GITHUB_BUILD=${SGF_GITHUB_BUILD}")

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
else ()
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
endif ()

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

# Optimization levels and debug info.
#
if (RELEASE)
    if (OS_EMSCRIPTEN)
        add_compile_options(-Os)
        add_link_options(-Os)
    elseif (OS_LINUX)
        add_compile_options(-O3)
    endif ()
elseif (DEBUG)
    if (OS_EMSCRIPTEN)
#        add_compile_options(-gsource-map)
#        add_link_options(-gsource-map)
        add_compile_options(-g3)
        add_link_options(-g3)
    endif ()
endif ()

if(OS_LINUX)
    set(ASAN_ENABLED 1)
endif()

if(ASAN_ENABLED)
    if(OS_WINDOWS)
        set(SANITIZER_OPTIONS /fsanitize=address)
    elseif(OS_LINUX)
        set(SANITIZER_OPTIONS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    else()
        message(FATAL_ERROR "Address sanitizer not supported for this target.")
    endif()
    add_compile_options(${SANITIZER_OPTIONS})
    add_link_options(${SANITIZER_OPTIONS})
    add_compile_definitions(ASAN_ENABLED=1)
    message("### Address sanitizer enabled: ${SANITIZER_OPTIONS}")
endif()

# Some miscellaneous emscripten settings.
#
if (OS_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(--shell-file "${CMAKE_SOURCE_DIR}/embuild/index_template.html")
    add_link_options("SHELL:--emrun -s TOTAL_MEMORY=64mb -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS=_main,_free")
endif ()

# Copy assets from 'source' target to 'target' target (usually an exe)
#
# Pretty ugly as it also checks for modified asset files but it (almost) seems to work.
#
# Also, asset files are only enumed at cmake config time, so if you add an asset you'll have to run cmake config again.
#
function(target_copy_assets target source)

    get_target_property(SRC_DIR ${source} SOURCE_DIR)
    get_target_property(DST_DIR ${target} BINARY_DIR)

    set(SRC_ASSETS_DIR "${SRC_DIR}/assets")
    set(DST_ASSETS_DIR "${DST_DIR}/assets")
    set(STAMP_FILE "${DST_DIR}/${source}_assets.stamp")
    file(GLOB_RECURSE SRC_FILES "${SRC_ASSETS_DIR}/*")

    #message("SRC_ASSETS_DIR: ${SRC_ASSETS_DIR}")
    #message("DST_ASSETS_DIR: ${DST_ASSETS_DIR}")
    #message("STAMP_FILE: ${STAMP_FILE}")
    #message("SRC_FILES: ${SRC_FILES}")

    # Create stamp files. We need to do this so GLOB_RECURSE in target_link_assets
    # can correctly enumerate all stampfile at cmake config time.
    execute_process(
            COMMAND "${CMAKE_COMMAND}" -E touch "${STAMP_FILE}"
    )

    # Copy assets dir and touch stampfile.
    add_custom_command(
            OUTPUT "${STAMP_FILE}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SRC_ASSETS_DIR}" "${DST_ASSETS_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E touch "${STAMP_FILE}"
            DEPENDS "${SRC_FILES}"
            VERBATIM
    )

    # Make target dependent on stampfile.
    set(assets_target ${target}_${source}_assets)
    add_custom_target(${assets_target} DEPENDS "${STAMP_FILE}")
    add_dependencies(${target} ${assets_target})

endfunction()

function(target_link_assets target)
    if (OS_EMSCRIPTEN)
        get_target_property(DST_DIR ${target} BINARY_DIR)
        file(GLOB_RECURSE STAMP_FILES "${DST_DIR}/*_assets.stamp")

        set_target_properties(${target} PROPERTIES LINK_DEPENDS "${STAMP_FILES}")
        target_link_options(${target} PRIVATE --preload-file "${DST_DIR}/assets@/")
    endif ()
endfunction()

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(audio)         # deps: core
add_subdirectory(core)
add_subdirectory(core3d)        # deps: window
add_subdirectory(emgdb)         # deps: core
add_subdirectory(fetch)         # deps: core
add_subdirectory(geom)          # deps: core
add_subdirectory(glfw)          # deps: opengl
add_subdirectory(image)         # deps: geom, stb
add_subdirectory(imgui)         # deps: window
add_subdirectory(opengl)        # deps: core
add_subdirectory(scene)         # deps: imgui, core3d
add_subdirectory(stb)           # deps: core
add_subdirectory(gltf)          # deps: scene stb
add_subdirectory(window)        # deps: glfw, image
add_subdirectory(websockets)    # deps: core
add_subdirectory(webxr)         # deps: core3d
add_subdirectory(xrscene)       # deps: scene, webxr

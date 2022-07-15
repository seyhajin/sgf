cmake_minimum_required(VERSION 3.8)

if (OS_EMSCRIPTEN OR OS_LINUX)
    return()
endif ()

##### openxr module #####

project(openxr)

add_library(openxr
        openxrcamera.cpp
        openxrcamera.h
        openxrsession.cpp
        openxrsession.h
        openxr.hh
        )

if (OS_WINDOWS)

    target_include_directories(openxr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/include"
            )

    target_link_libraries(openxr PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenxr/windows_x64/lib/openxr_loader.lib"
            glwindow
            )

else ()

    find_package(OpenXR REQUIRED)

    target_include_directories(openxr PUBLIC
            "${OpenXR_INCLUDE_DIR}"
            )

    target_link_libraries(openxr PUBLIC
            "${OpenXR_LIBRARY}"
            glwindow
            )

endif ()

##### openxr tests #####

add_executable(helloboxvr "tests/helloboxvr.cpp")

target_link_libraries(helloboxvr openxr scene)

copy_scene_assets_to_target(helloboxvr "${CMAKE_CURRENT_BINARY_DIR}/assets")

emscripten_preload_file(helloboxvr "${CMAKE_CURRENT_BINARY_DIR}/assets@/")

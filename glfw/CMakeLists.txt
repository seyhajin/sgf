cmake_minimum_required(VERSION 3.8)

##### glfw module #####

project(glfw)

add_library(glfw
        glfw.cpp
        glfw.hh
        )

target_compile_definitions(glfw PUBLIC GLFW_INCLUDE_NONE)

if (OS_EMSCRIPTEN)
    target_link_options(glfw PUBLIC "SHELL:-s USE_GLFW=3")
    target_link_libraries(glfw opengl)
elseif (OS_WINDOWS)
    target_include_directories(glfw PUBLIC "${PROJECT_SOURCE_DIR}/libglfw/include")
    target_link_libraries(glfw opengl "${PROJECT_SOURCE_DIR}/libglfw/windows_x64/lib/glfw3.lib")
else ()
    find_package(GLFW3 REQUIRED)
    target_include_directories(glfw PUBLIC "${GLFW3_INCLUDE_DIR}")
    target_link_libraries(glfw opengl "${GLFW3_LIBRARY}" X11)
endif ()

cmake_minimum_required(VERSION 3.8)

##### imgui module #####

project(imgui)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/libimgui")

add_library(imgui
        imgui.hh
        imguiex.cpp
        imguiex.h
        imguilog.cpp
        imguilog.h
        "${SOURCE_DIR}/ImGui/imgui.cpp"
        "${SOURCE_DIR}/ImGui/imgui.h"
        "${SOURCE_DIR}/ImGui/imgui_demo.cpp"
        "${SOURCE_DIR}/ImGui/imgui_draw.cpp"
        "${SOURCE_DIR}/ImGui/imgui_tables.cpp"
        "${SOURCE_DIR}/ImGui/imgui_widgets.cpp"
        "${SOURCE_DIR}/ImGui/imconfig.h"
        "${SOURCE_DIR}/ImGui/imgui_internal.h"
        "${SOURCE_DIR}/ImGui/imstb_rectpack.h"
        "${SOURCE_DIR}/ImGui/imstb_textedit.h"
        "${SOURCE_DIR}/ImGui/backends/imgui_impl_opengl3.cpp"
        "${SOURCE_DIR}/ImGui/backends/imgui_impl_opengl3.h"
        "${SOURCE_DIR}/ImGui/backends/imgui_impl_glfw.cpp"
        "${SOURCE_DIR}/ImGui/backends/imgui_impl_glfw.h"
        "${SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.cpp"
        "${SOURCE_DIR}/ImGuiFileDialog/ImGuiFileDialog.h"
        )

target_compile_definitions(imgui PUBLIC IMGUI_CMAKE_LISTS_INCLUDED)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<opengl/opengl.hh>)

if(USE_OPENGLES)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_ES3=1)
endif()
target_include_directories(imgui PUBLIC "${SOURCE_DIR}/ImGui" "${SOURCE_DIR}/ImGui/backends" "${SOURCE_DIR}/ImGuiFileDialog")
target_link_libraries(imgui window)

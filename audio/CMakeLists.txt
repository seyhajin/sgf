cmake_minimum_required(VERSION 3.8)

##### audio module #####

project(audio)

add_library(audio
        audio.cpp
        audio.h
        dr_wav.h
        )

if (OS_EMSCRIPTEN)
    target_link_libraries(audio core -lopenal)
elseif (OS_WINDOWS)
    target_include_directories(audio PRIVATE "${PROJECT_SOURCE_DIR}/libopenal/include")
    target_link_libraries(audio core "${PROJECT_SOURCE_DIR}/libopenal/windows_x64/lib/OpenAL32.lib")
else()
    find_package(OpenAL REQUIRED)
    target_include_directories(audio PRIVATE "${OPENAL_INCLUDE_DIR}")
    target_link_libraries(audio core "${OPENAL_LIBRARY}")
endif ()

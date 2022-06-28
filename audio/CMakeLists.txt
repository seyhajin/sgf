
cmake_minimum_required(VERSION 3.8)

project(audio)

add_library(audio
        audio.cpp
        audio.h
        dr_wav.h
        )

if (OS_EMSCRIPTEN)

    target_link_libraries(audio PUBLIC -lopenal)

elseif (OS_WINDOWS)

    target_include_directories(audio PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenal/include"
            )
    target_link_libraries(audio PUBLIC
            "${PROJECT_SOURCE_DIR}/libopenal/windows_x64/lib/OpenAL32.lib"
            )

elseif (OS_LINUX)

    find_package(OpenAL REQUIRED)

    target_include_directories(audio PUBLIC
            "${OPENAL_INCLUDE_DIR}"
            )
    target_link_libraries(audio PUBLIC
            "${OPENAL_LIBRARY}"
            core
            )

endif ()


cmake_minimum_required(VERSION 3.8)

if (OS_EMSCRIPTEN)
    return()
endif()

##### fetch module #####

project(fetch)

add_library(fetch
        fetch.cpp
        fetch.h
        fetch.hh
        )

if (OS_EMSCRIPTEN)

elseif (OS_WINDOWS)

    target_compile_definitions(fetch PRIVATE -DCURL_STATICLIB)

    target_include_directories(fetch PRIVATE
            "${PROJECT_SOURCE_DIR}/libcurl/include"
            )

    target_link_libraries(fetch PRIVATE
            "${PROJECT_SOURCE_DIR}/libcurl/windows_x64/release/libcurl.lib"
            core
            Ws2_32.lib
            crypt32.lib
            )
else ()

    find_package(CURL REQUIRED)

    target_include_directories(fetch PRIVATE
            "${CURL_INCLUDE_DIR}"
            )

    target_link_libraries(fetch PRIVATE
            "${CURL_LIBRARY}"
            core
            )
endif ()

##### tests #####

add_executable(fetchtest tests/fetchtest.cpp)
target_link_libraries(fetchtest PRIVATE fetch)

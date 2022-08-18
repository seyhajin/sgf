cmake_minimum_required(VERSION 3.8)

##### fetch module #####

project(fetch)

add_library(fetch
        fetch.cpp
        fetch.h
        fetch.hh
        )

if (OS_EMSCRIPTEN)
    target_link_libraries(fetch core)
elseif (OS_WINDOWS)
    target_compile_definitions(fetch PRIVATE -DCURL_STATICLIB)
    target_include_directories(fetch PRIVATE "${PROJECT_SOURCE_DIR}/libcurl/include")
    target_link_libraries(fetch core "${PROJECT_SOURCE_DIR}/libcurl/windows_x64/release/libcurl.lib" Ws2_32.lib crypt32.lib)
else ()
    find_package(CURL REQUIRED)
    target_include_directories(fetch PRIVATE "${CURL_INCLUDE_DIR}")
    target_link_libraries(fetch core "${CURL_LIBRARY}")
endif ()

##### fetch tests #####

add_executable(fetchtest tests/fetchtest.cpp)
target_link_libraries(fetchtest fetch)

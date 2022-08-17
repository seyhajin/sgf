cmake_minimum_required(VERSION 3.8)

# No windows websockets yet due to SSL issues.
if (SGF_GITHUB_BUILD OR OS_WINDOWS)
    return()
endif ()

##### websockets library #####

project(websockets)

set(SOURCES websockets.cpp websockets.h websockets.hh)

if (OS_EMSCRIPTEN)
    set(SOURCES ${SOURCES} websockets_emscripten.cpp websockets_emscripten.js)
else ()
    set(SOURCES ${SOURCES} websockets_desktop.cpp)
endif ()

add_library(websockets ${SOURCES})

if (OS_EMSCRIPTEN)
    em_link_js_library(websockets "${PROJECT_SOURCE_DIR}/websockets_emscripten.js")
    target_link_libraries(websockets core)
elseif (OS_LINUX)
    target_include_directories(websockets PRIVATE "${PROJECT_SOURCE_DIR}/libixwebsocket/include/ixwebsocket")
    target_link_libraries(websockets core "${PROJECT_SOURCE_DIR}/libixwebsocket/linux_x64/libixwebsocket.a" ssl crypto z)
endif ()

##### websocket tests #####

add_executable(websocketstest tests/websocketstest.cpp)
target_link_libraries(websocketstest websockets)

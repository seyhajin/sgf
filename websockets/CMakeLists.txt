cmake_minimum_required(VERSION 3.8)

##### webrtc #####

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

else ()

    target_include_directories(websockets PRIVATE "${PROJECT_SOURCE_DIR}/libixwebsocket/include/ixwebsocket")

    if (OS_LINUX)
        target_link_libraries(websockets PRIVATE
                "${PROJECT_SOURCE_DIR}/libixwebsocket/linux_x64/libixwebsocket.a"
                core ssl crypto z)
    endif ()

endif ()

##### websocket tests #####

add_executable(websocketstest tests/websocketstest.cpp)

target_link_libraries(websocketstest PRIVATE websockets)

cmake_minimum_required(VERSION 3.8)

if (SGF_GITHUB_BUILD)
    return()
endif ()

if(OS_WINDOWS)
    message("### Websockets not supported on windows due to numerous OpenSSL headaches.")
    return()
endif()

find_package(OpenSSL)
if (NOT OPENSSL_FOUND)
    message("### Oops, the websockets module cannot be built because the OpenSSL package was not found by cmake.")
    return()
endif ()

##### websockets module #####

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
elseif (OS_WINDOWS)
    # Maybe one day...
    target_include_directories(websockets PRIVATE "${PROJECT_SOURCE_DIR}/libixwebsocket/include/ixwebsocket")
    target_link_libraries(websockets core
            "${PROJECT_SOURCE_DIR}/libixwebsocket/windows_x64/ixwebsocket.lib"
            ${OPENSSL_LIBRARIES}
            Ws2_32.lib crypt32.lib shlwapi.lib
            )
elseif (OS_LINUX)
    target_include_directories(websockets PRIVATE "${PROJECT_SOURCE_DIR}/libixwebsocket/include/ixwebsocket" "${OPENSSL_INCLUDE_DIR}")
    target_link_libraries(websockets core
            "${PROJECT_SOURCE_DIR}/libixwebsocket/linux_x64/libixwebsocket.a"
            ${OPENSSL_LIBRARIES}
            )
endif ()

##### websocket tests #####

add_executable(websocketstest tests/websocketstest.cpp)
target_link_libraries(websocketstest websockets)

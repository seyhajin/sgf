cmake_minimum_required(VERSION 3.8)

###### window module #####

project(window)

add_library(window
        gamepad.cpp
        gamepad.h
        gamepadmapping.cpp
        gamepadmapping.h
        glwindow.cpp
        glwindow.h
        keyboard.cpp
        keyboard.h
        keycodes.cpp
        keycodes.h
        gamepadmappings.inc
        mouse.cpp
        mouse.h
        uidevice.cpp
        uidevice.h
        window.cpp
        window.h
        window.hh
        )

target_link_libraries(window glfw image)

##### window tests #####

add_executable(helloworld tests/helloworld.cpp)
target_link_libraries(helloworld window)

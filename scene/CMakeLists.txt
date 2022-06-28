
cmake_minimum_required(VERSION 3.8)

project(scene)

add_library(scene
        camera.cpp
        camera.h
        debugrenderer.cpp
        debugrenderer.h
        entity.cpp
        entity.h
        light.cpp
        light.h
        rendercontext.cpp
        rendercontext.h
        renderer.cpp
        renderer.h
        renderparams.cpp
        renderparams.h
        renderpass.cpp
        renderpass.h
        scene.cpp
        scene.h
        scene.hh
        shaderloader.cpp shaderloader.h mesh.cpp mesh.h material.cpp material.h textureloader.cpp textureloader.h)

target_link_libraries(scene core3d image)

##### scene tests #####

add_executable(hellobox
        tests/hellobox.cpp shaders/cameraparams.h)

target_link_libraries(hellobox scene glwindow)


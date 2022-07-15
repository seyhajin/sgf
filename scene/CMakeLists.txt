
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
        material.cpp
        material.h
        mesh.cpp
        mesh.h
        meshbuilder.cpp
        meshbuilder.h
        meshprimitives.cpp
        meshprimitives.h
        meshutils.cpp
        meshutils.h
        model.cpp
        model.h
        modelinstance.cpp
        modelinstance.h
        modelrenderer.cpp
        modelrenderer.h
        orthographiccamera.cpp
        orthographiccamera.h
        perspectivecamera.cpp
        perspectivecamera.h
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
        shaderloader.cpp
        shaderloader.h
        textureloader.cpp
        textureloader.h
        )

function(copy_scene_assets target)
    add_custom_command(TARGET ${target} PRE_LINK COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_PROPERTY:${target},BINARY_DIR>/assets/shaders")
endfunction()

target_link_libraries(scene core3d image)

##### scene tests #####

add_executable(hellobox tests/hellobox.cpp)
target_link_libraries(hellobox scene)
target_link_assets(hellobox)
copy_scene_assets(hellobox)

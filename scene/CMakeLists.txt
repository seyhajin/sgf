
cmake_minimum_required(VERSION 3.8)

project(scene)

add_library(scene
        actor.cpp
        actor.h
        camera.cpp
        camera.h
        collider.cpp
        collider.h
        collisionspace.cpp
        collisionspace.h
        contact.cpp
        contact.h
        debugrenderer.cpp
        debugrenderer.h
        entity.cpp
        entity.h
        intersect.cpp
        intersect.h
        light.cpp
        light.h
        linearspace.cpp
        linearspace.h
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
        spherecollider.cpp
        spherecollider.h
        textureloader.cpp
        textureloader.h

        assets/shaders/modelrenderer.glsl
        )

#function(target_copy_assets target)

#    add_custom_target(shaders DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/modelrenderer.glsl")
#    add_dependencies(target shaders)

#    add_custom_command(TARGET ${target} PRE_LINK COMMAND ${CMAKE_COMMAND} -E copy_directory
#            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
#            "$<TARGET_PROPERTY:${target},BINARY_DIR>/assets/shaders")
#endfunction()

target_link_libraries(scene core3d image)

# Bizzare way you're supposed to add a file depenedency in cmake!
#set(SHADERS_STAMP "${CMAKE_CURRENT_BINARY_DIR}/shaders.stamp")
#add_custom_command(
#        OUTPUT "${SHADERS_STAMP}"
#        COMMAND ${CMAKE_COMMAND} -E touch "${SHADERS_STAMP}"
#        DEPENDS "${PROJECT_SOURCE_DIR}/assets/shaders/modelrenderer.glsl")
#add_custom_target(shaders DEPENDS "${SHADERS_STAMP}")
#add_custom_target(shaders DEPENDS "${PROJECT_SOURCE_DIR}/assets/shaders/modelrenderer.glsl")
#add_dependencies(scene shaders)

##### scene tests #####

add_executable(hellobox tests/hellobox.cpp)
target_link_libraries(hellobox scene)
target_copy_assets(hellobox scene)
target_link_assets(hellobox)

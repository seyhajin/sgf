cmake_minimum_required(VERSION 3.8)

##### scene module #####

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
        modelrenderdata.cpp
        modelrenderdata.h
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
        shaderasset.cpp
        shaderasset.h
        spherecollider.cpp
        spherecollider.h
        textureasset.cpp
        textureasset.h
        meshcollider.cpp meshcollider.h meshcolliderdata.cpp meshcolliderdata.h rigidbody.cpp rigidbody.h)

target_link_libraries(scene imgui core3d)

##### scene tests #####

add_executable(hellobox tests/hellobox.cpp)
target_link_libraries(hellobox scene)
target_copy_assets(hellobox scene)
target_link_assets(hellobox)

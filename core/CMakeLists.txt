cmake_minimum_required(VERSION 3.8)

##### core module #####

project(core)

add_library(core
        any.cpp
        any.h
        appevent.cpp
        appevent.h
        bitmask.cpp
        bitmask.h
        core.cpp
        core.hh
        debug.cpp
        debug.h
        error.cpp
        error.h
        fileio.cpp
        fileio.h
        function.cpp
        function.h
        object.cpp
        object.h
        podvector.cpp
        podvector.h
        promise.cpp
        promise.h
        property.cpp
        property.h
        random.cpp
        random.h
        shared.cpp
        shared.h
        sharedptr.cpp
        sharedptr.h
        signal.cpp
        signal.h
        signal_impl.inl
        std.cpp
        std.h
        )

if (OS_EMSCRIPTEN)
    target_link_options(core PUBLIC "SHELL:-sNO_EXIT_RUNTIME=1 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['sgfRegistry','sgfRegisterObject','sgfGetObject','sgfDeregisterObject']")
    em_link_js_library(core "${PROJECT_SOURCE_DIR}/jsutils.js")
elseif (OS_LINUX)
    target_link_libraries(core pthread)
endif ()

add_subdirectory(tests)

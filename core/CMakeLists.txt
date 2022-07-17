
cmake_minimum_required(VERSION 3.8)

project(core)

##### core module #####

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
        weakptr.cpp
        weakptr.h
        )

##### tests #####

add_executable(promisetest tests/promisetest.cpp)
target_link_libraries(promisetest PRIVATE core)

add_executable(signaltest tests/signaltest.cpp)
target_link_libraries(signaltest PRIVATE core)

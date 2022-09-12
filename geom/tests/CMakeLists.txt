cmake_minimum_required(VERSION 3.8)

##### geom tests #####

add_executable(geomtests geomtests.cpp)
target_link_libraries(geomtests core)

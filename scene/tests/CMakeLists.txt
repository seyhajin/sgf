cmake_minimum_required(VERSION 3.8)

##### scene tests #####

add_executable(hellobox hellobox.cpp)
target_link_libraries(hellobox scene)

target_copy_assets(hellobox scene)
target_link_assets(hellobox)

add_executable(rigidbodytest rigidbodytest.cpp)
target_link_libraries(rigidbodytest scene)
target_copy_assets(rigidbodytest rigidbodytest)
target_copy_assets(rigidbodytest scene)
target_link_assets(rigidbodytest)

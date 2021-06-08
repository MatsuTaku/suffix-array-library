cmake_minimum_required(VERSION 3.17)

project(suffix_array_lib CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(suffix_array_lib INTERFACE)
target_include_directories(suffix_array_lib INTERFACE .)

add_executable(test test.cpp)
target_link_libraries(test suffix_array_lib)


cmake_minimum_required(VERSION 3.17.3)
project(SD3D)
set(CMAKE_CXX_STANDARD 20)

add_library(glad src/glad.c)

target_include_directories(glad PRIVATE include)

target_compile_options(glad PRIVATE -Wno-error)

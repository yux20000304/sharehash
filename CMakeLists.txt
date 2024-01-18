cmake_minimum_required(VERSION 3.20)
project(sharehash)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-g -O3)

include_directories(include)

add_executable(sharehash main.cpp)

add_executable(benchmark benchmark/benchmark.cpp)
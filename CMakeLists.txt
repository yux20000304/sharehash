cmake_minimum_required(VERSION 3.20)
project(sharehash)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-pg -O3)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(benchmark include ${Boost_INCLUDE_DIRS} )

add_executable(sharehash main.cpp)

add_executable(benchmark benchmark/benchmark.cpp)

target_link_libraries(benchmark Boost::program_options)
cmake_minimum_required(VERSION 3.20)
project(sharehash)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-pg -O3)

find_package(Boost COMPONENTS program_options thread REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

include_directories(benchmark/competitors include ${Boost_INCLUDE_DIRS} ${Threads_INCLUDE_DIRS} ${OpenMP_INCLUDE_DIRS})

add_executable(sharehash main.cpp)

add_executable(benchmark benchmark/benchmark.cpp)

target_link_libraries(benchmark Boost::program_options Boost::thread Threads::Threads OpenMP::OpenMP_CXX)
cmake_minimum_required(VERSION 3.15)

project(l1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# file(GLOB_RECURSE sources ./*.cpp)
set(sources foo.cpp)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(l1 ${sources})

target_link_libraries(l1 Boost::program_options)

cmake_minimum_required(VERSION 3.15)

project(l1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(z1 z1.cpp)
add_executable(z2 z2.cpp)
add_executable(z3 z3.cpp)

target_link_libraries(z1 Boost::program_options)
target_link_libraries(z2 Boost::headers)
target_link_libraries(z3 Boost::headers)

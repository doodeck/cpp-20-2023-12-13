cmake_minimum_required(VERSION 3.28)

project(cpp_modules_cmake)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(primes)
add_subdirectory(implementation-units)
add_subdirectory(module-partitions)
add_subdirectory(drawing-app)
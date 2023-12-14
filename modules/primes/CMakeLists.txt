cmake_minimum_required(VERSION 3.28)

project(primes)

# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now and it is not required for any other compiler

set(CMAKE_CXX_STANDARD 20)

add_library(primes_lib)

target_sources(primes_lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
    primes.cxx
)

add_executable(primes primes_main.cpp)
target_link_libraries(primes PRIVATE primes_lib)
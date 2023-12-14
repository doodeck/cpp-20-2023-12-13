cmake_minimum_required(VERSION 3.28)

project(module-partitions LANGUAGES CXX)

# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now and it is not required for any other compiler

set(CMAKE_CXX_STANDARD 20)

add_library(eshop_partitions_lib)

target_sources(eshop_partitions_lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
    eshop.cxx
    eshop_customer.cxx
    eshop_order.cxx
)

target_sources(eshop_partitions_lib 
  PRIVATE
    eshop_io_impl.cpp
    eshop_price_impl.cpp
)

add_executable(eshop_partitions eshop_main.cpp)
target_link_libraries(eshop_partitions PRIVATE eshop_partitions_lib)
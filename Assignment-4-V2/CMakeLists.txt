cmake_minimum_required(VERSION 3.23)
project(Assignment_4_V2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Assignment_4_V2
        main.cpp
        node.h
        routing_algo.cpp)

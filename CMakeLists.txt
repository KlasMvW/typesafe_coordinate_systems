cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(TS VERSION 0.1.0 LANGUAGES CXX)

add_library(TS INTERFACE)

target_include_directories(TS INTERFACE include)
target_sources(TS INTERFACE include/cs/typesafe_coordinate_systems.h)

add_subdirectory(test)  


cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

project(TS VERSION 0.2.0 LANGUAGES CXX)

enable_testing()

add_library(TS INTERFACE)

target_include_directories(TS INTERFACE include)
target_sources(TS INTERFACE include/ts/typesafe_coordinate_systems.h
                            include/ts/typesafe_system.h)

add_subdirectory(test)

# Create package version file and copy it to source folder.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_BINARY_DIR}/TSConfigVersion.cmake VERSION ${CMAKE_PROJECT_VERSION} COMPATIBILITY ExactVersion)
file(COPY ${CMAKE_BINARY_DIR}/TSConfigVersion.cmake DESTINATION ${CMAKE_SOURCE_DIR})



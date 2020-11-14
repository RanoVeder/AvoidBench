cmake_minimum_required(VERSION 3.14)
project(Avoid VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_BUILD_TYPE Release)

include(cmake/submodule.cmake)

add_subdirectory(Airsim/cmake)
add_subdirectory(AvoidLib)
add_subdirectory(Python)


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")



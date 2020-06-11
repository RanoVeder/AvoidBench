cmake_minimum_required(VERSION 3.14)
project(avoidlib)

set(CMAKE_CXX_STANDARD 14)

include(cmake/setup_dependencies.cmake)
include(cmake/AvoidLibClient.cmake)
include(cmake/AvoidLibServer.cmake)

add_subdirectory(test)

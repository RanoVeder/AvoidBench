cmake_minimum_required(VERSION 3.14)
project(AvoidPython VERSION 0.0.1)

add_subdirectory(pybind11)


add_library(avoid MODULE Avoid/Avoid.cpp)
target_link_libraries(avoid 
        PUBLIC avoidlib_client
        PRIVATE pybind11::module
    )
set_target_properties(avoid PROPERTIES 
        PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
set_target_properties(avoid PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
    )



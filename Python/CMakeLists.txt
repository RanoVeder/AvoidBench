cmake_minimum_required(VERSION 3.14)
project(AvoidPython VERSION 0.1.2)

add_subdirectory(pybind11)


# add_library(avoid MODULE Avoid/Avoid.cpp)
pybind11_add_module(avoidbench MODULE Avoid/Avoid.cpp)
target_link_libraries(avoidbench 
        PUBLIC avoidlib_client
        PRIVATE pybind11::module
    )
set_target_properties(avoidbench PROPERTIES 
        PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
set_target_properties(avoidbench PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib.win-amd64-3.6/
		LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/lib.win-amd64-3.6/
		LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/build/lib.win-amd64-3.6/
    )



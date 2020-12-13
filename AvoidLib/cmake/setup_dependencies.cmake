if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/rpclib/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/googletest/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT TARGET rpc)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/rpclib)
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/googletest)

set(EIGEN_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/eigen/)
SET(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/json/include)
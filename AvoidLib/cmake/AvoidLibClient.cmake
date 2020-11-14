set(CLIENT_SOURCES 
    src/Client.cpp
    src/World.cpp
    src/Vehicle.cpp
    src/Mission.cpp
    src/rpc/RPCClient.cpp
    src/Benchmarking/BenchmarkingInterface.cpp
)



add_library(${PROJECT_NAME}_client STATIC ${CLIENT_SOURCES})

target_link_libraries(${PROJECT_NAME}_client
    PRIVATE
        rpc
)

target_include_directories(${PROJECT_NAME}_client 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
		${EIGEN_INCLUDE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${JSON_INCLUDE_DIR}
		
)


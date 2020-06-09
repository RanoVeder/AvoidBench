set(CLIENT_SOURCES 
    src/Client.cpp
    src/rpc/RPCClient.cpp
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
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


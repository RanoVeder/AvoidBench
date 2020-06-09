set(SERVER_SOURCES 
    src/Client.cpp
    src/rpc/RPCClient.cpp
)

add_library(${PROJECT_NAME}_server STATIC ${CLIENT_SOURCES})

target_link_libraries(${PROJECT_NAME}_server
    PRIVATE
        rpc
)

target_include_directories(${PROJECT_NAME}_server 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


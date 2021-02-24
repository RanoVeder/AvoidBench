set(SERVER_SOURCES 
    src/rpc/RPCServer.cpp
    src/rpc/headers/RPCServer.h
)

add_library(${PROJECT_NAME}_server STATIC ${SERVER_SOURCES})

target_link_libraries(${PROJECT_NAME}_server
    PUBLIC
        rpc
)

target_include_directories(${PROJECT_NAME}_server 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_dependencies(${PROJECT_NAME}_server AirLib MavLinkCom)

set_target_properties(${PROJECT_NAME}_server PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/libs"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/libs"
)

add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${AIRSIM_ROOT}/Unreal/Plugins/AirSim/Source/AirLib/lib/x64/Release" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/libs"  /E/H)

add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${AIRSIM_ROOT}/Unreal/Plugins/AirSim/Source/AirLib/deps/rpclib/lib/x64/Release/rpc.lib" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/libs")

add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/rpc/headers" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/include/avoid"  /E/H)


add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/rpc/headers" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/include/avoid"  /E/H)

add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/rpclib/include/rpc" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/Avoid/Dependencies/include/rpc"  /E/H)
				
add_custom_command(TARGET ${PROJECT_NAME}_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Airsim/Unreal/Plugins/AirSim" "${CMAKE_SOURCE_DIR}/Unreal/AvoidUE4/Plugins/AirSim"  /E/H)





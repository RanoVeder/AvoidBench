set(BINARY avoid_test)

add_executable(${BINARY} main.cpp ClientTest.cpp)

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC avoidlib_client gtest)
link_libraries(${FreeImage_LIBRARIES})

add_executable(mumford_shah mumford_shah.cpp)
add_test(mumford_shah mumford_shah)
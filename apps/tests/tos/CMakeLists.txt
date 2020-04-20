link_libraries(${FreeImage_LIBRARIES})

add_executable(mumford_shah mumford_shah.cpp)
target_link_libraries(mumford_shah Fixtures::ImagePath)

add_test(mumford_shah mumford_shah)
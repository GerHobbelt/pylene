link_libraries(${FreeImage_LIBRARIES})
add_compile_definitions(MLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

add_executable(mumford_shah mumford_shah.cpp)
add_test(mumford_shah mumford_shah)
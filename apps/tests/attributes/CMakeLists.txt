link_libraries(${FreeImage_LIBRARIES})
add_compile_definitions(MLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

add_executable(meaningfullness meaningfullness.cpp)
add_test(meaningfullness meaningfullness)
link_libraries(${FreeImage_LIBRARIES})

add_executable(meaningfullness meaningfullness.cpp)
target_link_libraries(meaningfullness Fixtures::ImagePath)

add_test(meaningfullness meaningfullness)
link_libraries(-lfreeimage)
add_definitions(-DMLN_IMG_PATH="$(CMAKE_SOURCE_DIR)/img/")


add_executable(meaningfullness meaningfullness.cpp)
add_test(meaningfullness meaningfullness)
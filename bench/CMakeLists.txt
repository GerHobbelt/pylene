#SUBDIRS(labeling)
#link_libraries(${FreeImage_LIBRARIES})

add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  link_libraries(-pthread)
endif()

link_libraries(benchmark)
add_executable(BMNeighborhood   BMNeighborhood.cpp)
add_executable(BMRotation       BMRotation.cpp)

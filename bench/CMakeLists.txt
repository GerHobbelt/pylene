#SUBDIRS(labeling)
#link_libraries(${FreeImage_LIBRARIES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  link_libraries(-pthread)
endif()

link_libraries(benchmark)
add_executable(BMNeighborhood BMNeighborhood.cpp)

#SUBDIRS(labeling)

add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  link_libraries(-pthread)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -Rpass-missed=loop-vectorize -g")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-all")
endif()

link_libraries(benchmark)
add_executable(BMNeighborhood   BMNeighborhood.cpp)
add_executable(BMRotation       BMRotation.cpp)
add_executable(BMMorphers       BMMorphers.cpp)

target_link_libraries(BMRotation ${FreeImage_LIBRARIES})
target_link_libraries(BMMorphers ${FreeImage_LIBRARIES})

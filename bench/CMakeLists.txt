find_package(benchmark)
find_package(Threads)

add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-fopt-info")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
else ()
  # W4141: 'inline' : utilisé plusieurs fois in benchmark.h
  set(STANDALONE_COMPILE_FLAGS " ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4141")
endif ()

set(src_standalone
  BMErosion.cpp
  BMMorphers.cpp
  BMNeighborhood.cpp
  BMReference_Neighborhood.cpp
  BMReference_Linear.cpp
  )

set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})

link_libraries(benchmark::benchmark Threads::Threads)
link_libraries(Mln)
#link_libraries(-lbenchmark)

add_executable(BMErosion        BMErosion.cpp BMErosion_main.cpp)
add_executable(BMNeighborhood   BMNeighborhood.cpp BMNeighborhood_main.cpp)
add_executable(BMRotation       BMRotation.cpp)
add_executable(BMDilation       BMDilation.cpp)
add_executable(BMMorphers       BMMorphers.cpp   BMMorphers_main.cpp)
add_executable(BMReference_Linear BMReference_Linear.cpp BMReference_Linear_main.cpp)
add_executable(BMReference_Neighborhood BMReference_Neighborhood.cpp BMReference_Neighborhood_main.cpp)


target_link_libraries(BMRotation ${FreeImage_LIBRARIES})
target_link_libraries(BMMorphers ${FreeImage_LIBRARIES})
target_link_libraries(BMErosion ${FreeImage_LIBRARIES})
target_link_libraries(BMDilation ${FreeImage_LIBRARIES})
target_link_libraries(BMReference_Linear ${FreeImage_LIBRARIES})
target_link_libraries(BMReference_Neighborhood ${FreeImage_LIBRARIES})

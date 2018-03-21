find_package(benchmark)

add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  link_libraries(-lpthread)
endif()


set(src_standalone
  BMErosion.cpp
  BMMorphers.cpp
  BMNeighborhood.cpp
  BMReference.cpp
  BMReference_Linear.cpp
  BMReference_Neighborhood.cpp
  test_coro.cpp
  )




if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")

  set(STANDALONE_CORO_COMPILE_FLAGS "${STANDALONE_COMPILE_FLAGS} -fcoroutines-ts -stdlib=libc++ -fno-exceptions")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

  set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
  set_source_files_properties(test_coro.cpp PROPERTIES COMPILE_FLAGS "${STANDALONE_CORO_COMPILE_FLAGS}")
  link_libraries(-lc++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-g -fopt-info-vec -fopt-info-vec-missed ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
  set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
else ()
  # W4141: 'inline' : utilisé plusieurs fois in benchmark.h
  set(STANDALONE_COMPILE_FLAGS " ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4141")
  set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
endif ()



link_libraries(benchmark::benchmark)
link_libraries(Mln)
#link_libraries(-lbenchmark)

add_library(BMTest test_coro.cpp)
add_executable(BMErosion        BMErosion.cpp BMErosion_main.cpp)
add_executable(BMNeighborhood   BMNeighborhood.cpp BMNeighborhood_main.cpp)
add_executable(BMRotation       BMRotation.cpp)
add_executable(BMDilation       BMDilation.cpp)
add_executable(BMMorphers       BMMorphers.cpp   BMMorphers_main.cpp)
add_executable(BMReference_Linear BMReference_Linear_main.cpp BMReference_Linear.cpp test_coro.cpp)
add_executable(BMReference_Neighborhood BMReference_Neighborhood_main.cpp BMReference_Neighborhood.cpp)
target_link_libraries(BMReference_Neighborhood ${FreeImage_LIBRARIES})
target_link_libraries(BMReference_Linear ${FreeImage_LIBRARIES})
target_link_libraries(BMRotation ${FreeImage_LIBRARIES})
target_link_libraries(BMMorphers ${FreeImage_LIBRARIES})
target_link_libraries(BMErosion ${FreeImage_LIBRARIES})
target_link_libraries(BMDilation ${FreeImage_LIBRARIES})

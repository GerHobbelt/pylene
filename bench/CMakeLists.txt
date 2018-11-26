find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)

# Download file dependency
# Big images are stored at https://www.lrde.epita.fr/~mroynard/big_images/
file(DOWNLOAD "https://www.lrde.epita.fr/~mroynard/big_images/Space1_20MB.jpg" "${CMAKE_BINARY_DIR}/bench/Space1_20MB.jpg")


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-g -fopt-info -fopt-info-vec-missed -ftree-vectorize -ftree-vectorizer-verbose=2")
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


add_custom_target(benchmarks ALL COMMENT "Build all the benchmarks.")

add_custom_target(benchmarks-run
        DEPENDS benchmarks
        COMMENT "Build and run all the benchmarks.")

macro(add_benchmark Executable)
    set(Sources ${ARGN})
    add_executable(BM${Executable} ${Sources})

    target_link_libraries(BM${Executable} PRIVATE Pylene::Pylene benchmark::benchmark ${FreeImage_LIBRARIES})
    target_include_directories(BM${Executable} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    add_dependencies(benchmarks BM${Executable})

    add_custom_target(BM${Executable}_run
        COMMAND BM${Executable} --benchmark_out_format=json --benchmark_out=BM${Executable}.json
        DEPENDS BM${Executable}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        USES_TERMINAL)

    add_dependencies(benchmarks-run BM${Executable}_run)
endmacro(add_benchmark)

add_benchmark(Erosion                 BMErosion.cpp BMErosion_main.cpp)
add_benchmark(Neighborhood            BMNeighborhood.cpp BMNeighborhood_main.cpp)
add_benchmark(Rotation                BMRotation.cpp)
add_benchmark(Dilation                BMDilation.cpp)
add_benchmark(Morphers                BMMorphers.cpp BMMorphers_main.cpp)
add_benchmark(Reference_Linear        BMReference_Linear.cpp BMReference_Linear_Reversed.cpp BMReference_Linear_main.cpp)
add_benchmark(Reference_Neighborhood  BMReference_Neighborhood.cpp BMReference_Neighborhood_main.cpp)

add_executable(UTReference_Neighborhood BMReference_Neighborhood.cpp BMReference_Neighborhood_test.cpp)
target_link_libraries(UTReference_Neighborhood Pylene::Pylene GTest::Main GTest::GTest ${FreeImage_LIBRARIES})

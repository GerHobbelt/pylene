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


add_custom_target(benchmarks-compile ALL COMMENT "Build all the benchmarks.")

add_custom_target(benchmarks
        DEPENDS benchmarks-compile
        COMMENT "Build and run all the benchmarks.")

macro(add_benchmark Executable Sources)
    string(REPLACE " " ";" SOURCES_LIST ${Sources})
    add_executable(BM${Executable}.exe EXCLUDE_FROM_ALL ${SOURCES_LIST})

    target_link_libraries(BM${Executable}.exe PRIVATE Mln benchmark::benchmark Threads::Threads ${FreeImage_LIBRARIES})
    target_include_directories(BM${Executable}.exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    add_dependencies(benchmarks-compile BM${Executable}.exe)

    add_custom_target(BM${Executable}
        COMMAND BM${Executable}.exe
        DEPENDS BM${Executable}.exe
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bench
        USES_TERMINAL)
        
    add_dependencies(benchmarks BM${Executable})
endmacro(add_benchmark)

add_benchmark(Erosion                 "BMErosion.cpp BMErosion_main.cpp")
add_benchmark(Neighborhood            "BMNeighborhood.cpp BMNeighborhood_main.cpp")
add_benchmark(Rotation                "BMRotation.cpp")
add_benchmark(Dilation                "BMDilation.cpp")
add_benchmark(Morphers                "BMMorphers.cpp BMMorphers_main.cpp")
add_benchmark(Reference_Linear        "BMReference_Linear.cpp BMReference_Linear_main.cpp")
add_benchmark(Reference_Neighborhood  "BMReference_Neighborhood.cpp BMReference_Neighborhood_main.cpp")

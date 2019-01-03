find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)

include(CTest)
include(ExternalData)
include(BenchmarkMacros.cmake)

# Download file dependency
set(ExternalData_URL_TEMPLATES
  "file:///lrde/dload/olena/pylene/data/%(algo)/%(hash)"
  "https://www.lrde.epita.fr/dload/olena/pylene/data/%(algo)/%(hash)")

ExternalData_Expand_Arguments(
  fetch-external-data
  images
  DATA{Space1_20MB.jpg}
  )

# Extra compiler options

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



enable_testing()

add_benchmark(BMNeighborhood            BMNeighborhood.cpp BMNeighborhood_main.cpp)
add_benchmark(BMRotation                BMRotation.cpp)
add_benchmark(BMDilation                BMDilation.cpp)
add_benchmark(BMMorphers                BMMorphers.cpp BMMorphers_main.cpp)
add_benchmark(BMReference_Linear        BMReference_Linear.cpp BMReference_Linear_Reversed.cpp BMReference_Linear_main.cpp)
add_benchmark(BMReference_Neighborhood  BMReference_Neighborhood.cpp BMReference_Neighborhood_main.cpp)

add_executable(UTReference_Neighborhood BMReference_Neighborhood.cpp BMReference_Neighborhood_test.cpp)
target_link_libraries(UTReference_Neighborhood PRIVATE Fixtures::ImagePath Pylene::Pylene GTest::Main GTest::GTest ${FreeImage_LIBRARIES})


ExternalData_Add_Target(fetch-external-data)

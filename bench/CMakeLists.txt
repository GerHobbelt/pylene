project(bench)

find_package(benchmark REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ExternalData)
include(BenchmarkMacros)

# Download file dependency
set(ExternalData_URL_TEMPLATES
  "file:///lrde/dload/olena/pylene/data/%(algo)/%(hash)"
  "https://www.lrde.epita.fr/dload/olena/pylene/data/%(algo)/%(hash)")

ExternalData_Expand_Arguments(
  fetch-external-data
  images
  DATA{Space1_20MB.jpg}
  DATA{Aerial_view_of_Olbia.jpg}
  )

# Extra compiler options

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -g")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-g -fopt-info-vec -fopt-info-vec-missed -ftree-vectorize -ftree-vectorizer-verbose=2")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -g")
else ()
  # W4141: 'inline' : utilisé plusieurs fois in benchmark.h
  set(STANDALONE_COMPILE_FLAGS " ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4141")
endif ()

add_library(BenchImpl
  include/bench/neighborhood.hpp
  src/neighborhood.cpp)


set_target_properties(BenchImpl PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
target_include_directories(BenchImpl PUBLIC include)
target_link_libraries(BenchImpl PUBLIC Pylene::Pylene)


add_subdirectory(tests)

set(src_standalone
  BMErosion.cpp
  BMMorphers.cpp
  BMNeighborhood.cpp
  BMReference_Neighborhood.cpp
  BMReference_Linear.cpp
  BMAlgorithms.cpp
  )

set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})


add_benchmark(BMAlgorithms              BMAlgorithms.cpp BMAlgorithms_main.cpp)
add_benchmark(BMMorphoMaxtree           BMMorphoMaxtree.cpp)
add_benchmark(BMMorphoTos               BMMorphoTos.cpp)
add_benchmark(BMMorphoBase              BMMorphoBase.cpp)
add_benchmark(BMMorphers                BMMorphers.cpp BMMorphers_main.cpp)
add_benchmark(BMReference_Linear        BMReference_Linear.cpp BMReference_Linear_Reversed.cpp BMReference_Linear_main.cpp)
add_benchmark(BMReference_Neighborhood  BMReference_Neighborhood_main.cpp)


find_package(OpenCV)

if (OpenCV_FOUND)
  add_benchmark(BMPlnVsOpenCV_Dilation          opencv/BMPlnVsOpenCV_dilation.cpp)
  target_link_libraries(BMPlnVsOpenCV_Dilation  PRIVATE ${OpenCV_LIBS})

  add_benchmark(BMPlnVsOpenCV_BgSubPipeline         opencv/BMPlnVsOpenCV_bgSubPipeline.cpp)
  target_link_libraries(BMPlnVsOpenCV_BgSubPipeline PRIVATE ${OpenCV_LIBS})
  add_sanitizers(BMPlnVsOpenCV_BgSubPipeline)
else()
  message("OpenCV not found: not building BMPlnVsOpenCV_Dilation nor BMPlnVsOpenCV_bgSubPipeline")
endif()


ExternalData_Add_Target(fetch-external-data)

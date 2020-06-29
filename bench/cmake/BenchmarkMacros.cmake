# .rst:
#
# This file defines the following functions:
#
# * add_benchmark(Executable src_1 src_2 ... src_n)
#
# It creates a target with the name BM${Executable} which is the binary file
# And a target BM${Executable}_run that runs the benchmark and is added to
# the list of executables to be run.
# A test is also added with the same name and belongs to the group "SpeedTests"
# They can be run with ctest -L SpeedTests.
# 

add_custom_target(run-all-benchmarks)


function(add_benchmark Executable)
  set(Benchmark_SOURCES ${ARGN})
  set(Benchmark_NAME ${Executable})

  # Create benchmark exe
  add_executable(${Benchmark_NAME} ${Benchmark_SOURCES})
  target_link_libraries(${Benchmark_NAME} PRIVATE Fixtures::ImagePath BenchImpl Pylene::Pylene benchmark::benchmark ${FreeImage_LIBRARIES})
  target_include_directories(${Benchmark_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_dependencies(build-bench ${Benchmark_NAME})

  # Retrieve reference
  set(Benchmark_OUTPUT_FILE ${Benchmark_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}.json)


  # Create a target to run the benchmark
  add_custom_target(run-${Benchmark_NAME}
                    COMMAND ${Benchmark_NAME} --benchmark_out_format=json --benchmark_out=${Benchmark_OUTPUT_FILE} --benchmark_repetitions=9 #--benchmark_display_aggregates_only=true
                    DEPENDS ${Benchmark_NAME}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    USES_TERMINAL)
  add_dependencies(run-all-benchmarks run-${Benchmark_NAME})

  # Create a speed test to run the benchmark and compare to the reference
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/references/${Benchmark_OUTPUT_FILE}.md5")
    message(STATUS "Test ${Benchmark_NAME} found.")
    externaldata_add_test(
        fetch-external-data
        NAME ${Benchmark_NAME}
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/compare.py --display_aggregates_only benchmarks DATA{references/${Benchmark_OUTPUT_FILE}} ${Benchmark_OUTPUT_FILE})
    set_tests_properties(${Benchmark_NAME} PROPERTIES
                         LABELS SpeedTests
                         RUN_SERIAL Yes)
  endif ()

endfunction(add_benchmark)

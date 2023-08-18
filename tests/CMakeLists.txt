project(tests)

find_package(Threads REQUIRED)
find_package(GTest 1.10 REQUIRED)
find_package(TBB REQUIRED tbb)

# Register an executable test: add_core_test(Exe Source1 [Source2...])
# Executable: The name of the executable (default: ${Prefix}${TestName})
# Sources: The source files to compile (default: ${Prefix}${TestName}.cpp)
function(add_core_test Executable)
  set(core_test_SOURCES ${ARGN})
  set(core_test_NAME ${Executable})

  add_executable(${core_test_NAME} ${core_test_SOURCES})
  target_link_libraries(${core_test_NAME} PRIVATE Fixtures::ImagePath Fixtures::ImageCompare Pylene::Core Pylene::IO-freeimage GTest::Main)
  set_target_properties(${core_test_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

  add_test(NAME ${core_test_NAME} COMMAND ${core_test_NAME} --gtest_output=xml:${core_test_NAME}.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
  set_tests_properties(${core_test_NAME} PROPERTIES LABELS UnitTests)
  add_dependencies(build-tests ${core_test_NAME})
endfunction(add_core_test)



include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(accu)
add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(colors)
add_subdirectory(morpho)
add_subdirectory(labeling)
add_subdirectory(contrib)
add_subdirectory(bp)
add_subdirectory(scribo)
add_subdirectory(transforms)
if (PYLENE_BUILD_PYTHON)
  add_subdirectory(python)
endif()

project(tests)

find_package(Threads REQUIRED)
find_package(GTest MODULE REQUIRED)
find_package(Sanitizers REQUIRED)

sanitizer_add_blacklist_file(${CMAKE_SOURCE_DIR}"/utils/sanitizers/blacklist.txt")

include_directories(${GTEST_INCLUDE_DIRS})

# Register an executable test: add_core_test(Exe Source1 [Source2...])
# Executable: The name of the executable (default: ${Prefix}${TestName})
# Sources: The source files to compile (default: ${Prefix}${TestName}.cpp)
function(add_core_test Executable)
  set(core_test_SOURCES ${ARGN})
  set(core_test_NAME ${Executable})

  add_executable(${core_test_NAME} ${core_test_SOURCES})
  add_test(NAME ${core_test_NAME} COMMAND ${core_test_NAME} --gtest_output=xml:${core_test_NAME}.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
  target_link_libraries(${core_test_NAME} PRIVATE Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene GTest::GTest GTest::Main ${FreeImage_LIBRARIES})
  set_tests_properties(${core_test_NAME} PROPERTIES LABELS UnitTests)
  
  add_sanitizers(${core_test_NAME})
  add_dependencies(build-tests ${core_test_NAME})

endfunction(add_core_test)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(accu)
add_subdirectory(core)
add_subdirectory(draw)
add_subdirectory(io)
add_subdirectory(colors)
add_subdirectory(morpho)
add_subdirectory(labeling)
add_subdirectory(graph)
add_subdirectory(graphcut)
add_subdirectory(contrib)

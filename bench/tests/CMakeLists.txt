project(tests)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(UTBenchImpl_Neighborhood neighborhood.cpp)
target_link_libraries(UTBenchImpl_Neighborhood PRIVATE BenchImpl Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene GTest::GTest GTest::Main ${FreeImage_LIBRARIES})

add_test(NAME UTBenchImpl_Neighborhood COMMAND UTBenchImpl_Neighborhood --gtest_output=xml:UTBenchImpl_Neighborhood.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bench/tests)

set_tests_properties(UTBenchImpl_Neighborhood PROPERTIES LABELS UnitTests)

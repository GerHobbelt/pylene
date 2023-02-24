include(CTest)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)


add_executable(UTBenchImpl_Neighborhood neighborhood.cpp)
target_link_libraries(UTBenchImpl_Neighborhood PRIVATE BenchImpl Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene GTest::gtest ${FreeImage_LIBRARIES})

add_test(NAME UTBenchImpl_Neighborhood COMMAND UTBenchImpl_Neighborhood --gtest_output=xml:UTBenchImpl_Neighborhood.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bench/tests)

set_tests_properties(UTBenchImpl_Neighborhood PROPERTIES LABELS UnitTests)

add_dependencies(build-tests UTBenchImpl_Neighborhood)

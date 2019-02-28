find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(Neighborhood_tests neighborhood.cpp)
target_link_libraries(Neighborhood_tests PRIVATE Neighborhood Fixtures::ImagePath Pylene::Pylene GTest::GTest GTest::Main ${FreeImage_LIBRARIES})
target_include_directories(Neighborhood_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)

add_test(NAME Neighborhood_tests COMMAND Neighborhood_tests --gtest_output=xml:Neighborhood_tests.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

add_dependencies(check-internals Neighborhood_tests)

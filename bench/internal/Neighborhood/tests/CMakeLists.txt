find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(UTInternal_Neighborhood neighborhood.cpp)
target_link_libraries(UTInternal_Neighborhood PRIVATE Neighborhood Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene GTest::GTest GTest::Main ${FreeImage_LIBRARIES})

add_test(NAME UTInternal_Neighborhood COMMAND UTInternal_Neighborhood --gtest_output=xml:UTInternal_Neighborhood.xml WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bench/internal)

add_dependencies(check-internals UTInternal_Neighborhood)
set_tests_properties(UTInternal_Neighborhood PROPERTIES LABELS InternalTests)

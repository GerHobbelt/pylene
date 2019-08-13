find_package(GTest REQUIRED)

add_library(ImageCompare)

target_include_directories(ImageCompare PUBLIC include)
target_link_libraries(ImageCompare PUBLIC Pylene::Pylene GTest::GTest)

target_sources(ImageCompare PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/fixtures/ImageCompare/image_compare.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/image_compare.cpp)


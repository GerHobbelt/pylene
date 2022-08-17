find_package(GTest REQUIRED)
find_package(fmt 6.0 REQUIRED)

add_library(ImageCompare)

target_include_directories(ImageCompare PUBLIC include)
target_link_libraries(ImageCompare PUBLIC Anonymous::Anonymous GTest::GTest fmt::fmt)
target_compile_features(ImageCompare PUBLIC cxx_std_20)

target_sources(ImageCompare PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/fixtures/ImageCompare/image_compare.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/image_compare.cpp)


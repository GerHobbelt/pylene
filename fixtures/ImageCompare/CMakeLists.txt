find_package(GTest REQUIRED)

add_library(ImageCompare INTERFACE)

target_compile_features(ImageCompare INTERFACE cxx_std_17)
target_include_directories(ImageCompare INTERFACE include)
target_link_libraries(ImageCompare INTERFACE Pylene::Pylene GTest::GTest)

target_sources(ImageCompare INTERFACE
               ${CMAKE_CURRENT_SOURCE_DIR}/include/fixtures/ImageCompare/image_compare.hpp)

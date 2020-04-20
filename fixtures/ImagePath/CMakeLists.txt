configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/image_path.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/src/image_path.cpp
               @ONLY)

add_library(ImagePath STATIC
            include/fixtures/ImagePath/image_path.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/src/image_path.cpp)

target_compile_features(ImagePath PUBLIC cxx_std_17)
target_include_directories(ImagePath PUBLIC include)

target_sources(ImagePath PUBLIC
               ${CMAKE_CURRENT_SOURCE_DIR}/include/fixtures/ImagePath/image_path.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/src/image_path.cpp.in)

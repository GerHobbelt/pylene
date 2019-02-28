add_library(Neighborhood STATIC
              include/fixtures/Neighborhood/neighborhood.hpp
              src/neighborhood.cpp)

target_compile_features(Neighborhood PUBLIC cxx_std_17)
target_include_directories(Neighborhood PUBLIC include)
target_link_libraries(Neighborhood PUBLIC Pylene::Pylene)

target_sources(Neighborhood PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include/fixtures/Neighborhood/neighborhood.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/neighborhood.cpp)

add_library(Neighborhood STATIC
              include/internal/Neighborhood/neighborhood.hpp
              src/neighborhood.cpp)

target_compile_features(Neighborhood PUBLIC cxx_std_17)
set_target_properties(Neighborhood PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
target_include_directories(Neighborhood PUBLIC include)
target_link_libraries(Neighborhood PUBLIC Pylene::Pylene)

target_sources(Neighborhood PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/Neighborhood/neighborhood.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/neighborhood.cpp)

include(CTest)
if (BUILD_TESTING)
   enable_testing()

   add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()

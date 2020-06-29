project(PyleneTest)
cmake_minimum_required(VERSION 3.11)

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
endif()

find_package(Pylene REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Pylene::Pylene)

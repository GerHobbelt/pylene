project(PyleneTest)
cmake_minimum_required(VERSION 3.8.2)

include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")

find_package(Pylene REQUIRED)
find_package(FreeImage REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Pylene::Pylene ${FreeImage_LIBRARIES})
target_include_directories(main PRIVATE ${FreeImage_INCLUDE_DIRS})

project(PyleneTest)
cmake_minimum_required(VERSION 3.11)

# Conan's 'cmake' generator is used only for pybind11. Will be remove
# when the cmake_find_package generator will work for pybind11
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  conan_set_rpath()
endif()

find_package(Pylene REQUIRED COMPONENTS Pylene)

add_executable(main main.cpp)
target_link_libraries(main Pylene::Pylene)

if (WITH_PYLENE_NUMPY)
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CONAN_BUILD_DIRS_PYBIND11}") # TO REMOVE WHEN cmake_find_package FOR PYBIND11 WORKS
  include(pybind11Install) # TO REMOVE WHEN cmake_find_package FOR PYBIND11 WORKS
  find_package(Pylene REQUIRED COMPONENTS Pylene-numpy)

  pybind11_add_module(pylene_extension)
  target_sources(pylene_extension PRIVATE pylene_extension.cpp)
  target_link_libraries(pylene_extension PRIVATE Pylene::Pylene-numpy) # CONAN_LIBS_PYBIND11 TO REMOVE WHEN cmake_find_package FOR PYBIND11 WORKS
  file(COPY ${CMAKE_SOURCE_DIR}/main.py DESTINATION ${CMAKE_BINARY_DIR})
endif(WITH_PYLENE_NUMPY)
project(PyleneTest)
cmake_minimum_required(VERSION 3.11)

find_package(Pylene REQUIRED COMPONENTS Pylene)

add_executable(main main.cpp)
target_link_libraries(main Pylene::Pylene)

if (WITH_PYLENE_NUMPY)
  find_package(Pylene REQUIRED COMPONENTS Pylene-numpy)
  find_package(pybind11 REQUIRED)

  pybind11_add_module(pylene_extension)
  target_sources(pylene_extension PRIVATE pylene_extension.cpp)
  target_link_libraries(pylene_extension PRIVATE Pylene::Pylene-numpy)
  file(COPY ${CMAKE_SOURCE_DIR}/main.py DESTINATION ${CMAKE_BINARY_DIR})
endif(WITH_PYLENE_NUMPY)
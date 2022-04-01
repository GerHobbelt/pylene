project(PyleneTest)
cmake_minimum_required(VERSION 3.11)

find_package(pylene REQUIRED COMPONENTS core io-freeimage)

add_executable(main main.cpp)
target_link_libraries(main pylene::core pylene::io-freeimage)

if (WITH_PYLENE_NUMPY)
  find_package(pybind11 REQUIRED)
  find_package(pylene REQUIRED COMPONENTS pylene-numpy)
  pybind11_add_module(pylene_extension)
  target_sources(pylene_extension PRIVATE pylene_extension.cpp)
  target_link_libraries(pylene_extension PRIVATE pylene::pylene-numpy)
  file(COPY ${CMAKE_SOURCE_DIR}/main.py DESTINATION ${CMAKE_BINARY_DIR})
endif(WITH_PYLENE_NUMPY)

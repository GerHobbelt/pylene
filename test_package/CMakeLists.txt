project(PyleneTest)
cmake_minimum_required(VERSION 3.11)

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
endif()

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CONAN_BUILD_DIRS_PYBIND11}")
  include(pybind11Install)
endif()

find_package(Pylene COMPONENTS Pylene REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main Pylene::Pylene)

if (WITH_PYLENA_NUMPY)
  find_package(Pylene COMPONENTS pylena_numpy REQUIRED)
  pybind11_add_module(pylene_extension pylene_extension.cpp)
  target_link_libraries(pylene_extension PRIVATE Pylene::Pylene Pylene::pylena_numpy)
  target_include_directories(pylene_extension PRIVATE ${CONAN_INCLUDE_DIRS_PYBIND11})
  file(COPY ${CMAKE_SOURCE_DIR}/main.py DESTINATION ${CMAKE_BINARY_DIR})
endif(WITH_PYLENA_NUMPY)
find_package(pybind11 REQUIRED)

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene)
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp
                              src/core/image_cast.cpp
                              src/core/numpy_format.cpp)
if (NOT CMAKE_POSITION_INDEPENDENT_CODE)
        message(WARNING "Building the Python components without POSITION_INDEPENDENT_CODE enabled. \
                         Build should fail during the linking part of the compilation of the pylena module.")
endif()

find_package(fmt 6.0 REQUIRED)
find_package(pybind11 REQUIRED)

add_library(Pylene-numpy OBJECT)
add_library(Pylene::Pylene-numpy ALIAS Pylene-numpy)
target_include_directories(Pylene-numpy PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:include>
)
target_sources(Pylene-numpy PRIVATE src/core/image_cast.cpp
                                    src/core/numpy_format.cpp)

target_link_libraries(Pylene-numpy PUBLIC Pylene::Core)
target_link_libraries(Pylene-numpy PRIVATE fmt::fmt pybind11::headers pybind11::python_headers)

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene::Pylene-numpy)
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp)

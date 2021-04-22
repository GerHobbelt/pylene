if (NOT CMAKE_POSITION_INDEPENDENT_CODE)
        message(WARNING "Building the Python components without POSITION_INDEPENDENT_CODE enabled. \
                         Build should fail during the linking part of the compilation of the pylena module.")
endif()

find_package(fmt 6.0 REQUIRED)
find_package(pybind11 REQUIRED)

add_library(Pylene-numpy)
add_library(Pylene::Pylene-numpy ALIAS Pylene-numpy)
target_include_directories(Pylene-numpy PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:include>
)
target_sources(Pylene-numpy PRIVATE src/core/image_cast.cpp
                                    src/core/numpy_format.cpp)

target_link_libraries(Pylene-numpy PUBLIC Pylene ${pybind11_python_link_helper_LINK_LIBS})
target_link_libraries(Pylene-numpy PRIVATE fmt::fmt)
target_include_directories(Pylene-numpy PRIVATE ${pybind11_INCLUDE_DIRS})

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene Pylene-numpy)
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp)

include(GNUInstallDirs)
install(TARGETS Pylene-numpy
        EXPORT PyleneTargets)

install(DIRECTORY include/pln
        TYPE INCLUDE)
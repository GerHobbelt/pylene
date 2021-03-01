pybind11_find_import(numpy REQUIRED)

add_library(Pylene-numpy)
add_library(Pylene::Pylene-numpy ALIAS Pylene-numpy)
target_include_directories(Pylene-numpy PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:include>
                                        # PYBIND11 INCLUDES TO REMOVE WHEN cmake_find_package FOR PYBIND11 WILL BE FIXED
                                        PUBLIC
                                               ${CONAN_INCLUDE_DIRS_PYBIND11}
                                               ${PYTHON_INCLUDE_DIRS})
target_sources(Pylene-numpy PRIVATE src/core/image_cast.cpp
                                    src/core/numpy_format.cpp)
# REPLACE PYTHON_LIBRARIES BY pybind11::pybind11 WHEN cmake_find_package FOR PYBIND11 WILL BE FIXED
target_link_libraries(Pylene-numpy PUBLIC Pylene ${PYTHON_LIBRARIES})

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene Pylene-numpy ${PYTHON_LIBRARIES})
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp)

include(GNUInstallDirs)
install(TARGETS Pylene-numpy
        EXPORT PyleneTargets)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/pln/core/image_cast.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pln/core)
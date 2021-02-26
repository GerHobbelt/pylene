pybind11_find_import(numpy REQUIRED)

add_library(Pylena_numpy)
add_library(Pylene::Pylena_numpy ALIAS Pylena_numpy)
target_include_directories(Pylena_numpy PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                               $<INSTALL_INTERFACE:include>
                                        # PRIVATE TO REMOVE WHEN cmake_find_package FOR PYBIND11 WILL BE FIXED
                                        PRIVATE
                                               ${CONAN_INCLUDE_DIRS_PYBIND11}
                                               ${PYTHON_INCLUDE_DIRS})
target_sources(Pylena_numpy PRIVATE src/core/image_cast.cpp
                                    src/core/numpy_format.cpp)
target_link_libraries(Pylena_numpy PUBLIC Pylene)

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene Pylena_numpy)
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp)

include(GNUInstallDirs)
install(TARGETS Pylena_numpy
        EXPORT PyleneTargets)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/pln/core/image_cast.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pln/core)
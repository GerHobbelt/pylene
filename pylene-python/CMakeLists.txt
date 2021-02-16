add_library(pylena_numpy)
target_include_directories(pylena_numpy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS_PYBIND11} ${PYTHON_INCLUDE_DIRS})
target_sources(pylena_numpy PRIVATE src/core/image_cast.cpp
                              src/core/numpy_format.cpp)
target_link_libraries(pylena_numpy PRIVATE Pylene)

pybind11_add_module(pylena)
target_link_libraries(pylena PRIVATE Pylene pylena_numpy)
target_include_directories(pylena PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(pylena PRIVATE src/module.cpp)

include(GNUInstallDirs)
install(TARGETS pylena_numpy
        EXPORT PyleneTargets)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/pln/core/image_cast.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pln/core)
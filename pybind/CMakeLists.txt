project(Pybind CXX)

find_package(pybind11 REQUIRED)

pybind11_add_module(Pylena MODULE
                    src/pylena.cpp
                    src/morpho.cpp
                    src/ndimage.cpp
                    src/ndimage_buffer_helper.cpp
                    )

target_link_libraries(Pylena PRIVATE Pylene::Pylene)
target_include_directories(Pylena PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_compile_features(Pylena PRIVATE cxx_std_20)

include(CTest)

add_custom_target(run-all-pytests)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

function(add_pytest name python_file_path)
  # Create a target to run the benchmark
  add_custom_target(run-${name}
                    COMMAND py.test ${python_file_path} --junitxml=${CMAKE_CURRENT_BINARY_DIR}/${name}.xml
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    USES_TERMINAL
                    )

  add_dependencies(run-all-pytests run-${name})
endfunction(add_pytest)

add_pytest(UTPy_Image ${CMAKE_CURRENT_SOURCE_DIR}/tests/ndimage.py)

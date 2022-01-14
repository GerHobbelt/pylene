project(Pybind CXX)

find_package(pybind11 REQUIRED)
find_package(fmt 6.0 REQUIRED)

pybind11_add_module(pylena MODULE
                    src/pylena.cpp
                    src/morpho.cpp
                    src/ndimage.cpp
                    src/se.cpp
                    src/py_value_set.cpp
                    )

target_link_libraries(pylena PRIVATE Pylene::Pylene fmt::fmt)
target_include_directories(pylena PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
target_compile_features(pylena PRIVATE cxx_std_20)
target_compile_options(pylena PRIVATE -Wno-deprecated-declarations) # https://github.com/pybind/pybind11/issues/1444

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")

#add_dependencies(build-pybind pylena)

# add_sanitizers(pylena)

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

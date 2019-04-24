project(Pybind CXX)

find_package(pybind11 REQUIRED)

pybind11_add_module(PyPylene MODULE 
                    src/pypylene.cpp
                    src/numpy_helper.cpp
                    )

target_link_libraries(PyPylene PRIVATE Pylene::Pylene)
target_include_directories(PyPylene PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)
target_compile_features(PyPylene PRIVATE cxx_std_17)


enable_testing()

add_custom_target(run-all-pytests)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

function(add_pytest name python_file_path)
  # Create a target to run the benchmark
  add_custom_target(run-${name}
    COMMAND ${Python3_EXECUTABLE} ${python_file_path}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    )

  add_dependencies(run-all-pytests run-${name})
endfunction(add_pytest)

add_pytest(image2d      ${CMAKE_CURRENT_SOURCE_DIR}/tests/image2d.py)

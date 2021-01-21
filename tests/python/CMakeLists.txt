find_package(pybind11 REQUIRED)
pybind11_find_import(numpy REQUIRED)

# Python module for testing
set(PYTHON_PYLENE_SRC ${CMAKE_SOURCE_DIR}/pylene-python/src)
pybind11_add_module(pylena_test pylena_test.cpp
                                ${PYTHON_PYLENE_SRC}/core/image_cast.cpp
                                ${PYTHON_PYLENE_SRC}/core/numpy_format.cpp)
target_link_libraries(pylena_test PRIVATE Pylene ImagePath)
target_include_directories(pylena_test PRIVATE ${CMAKE_SOURCE_DIR}/pylene-python/include)

function(add_python_test filename)
    file(COPY ${filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endfunction(add_python_test)

# PYTHON FILES TO MOVE HERE
add_python_test(test_pylena_numpy.py)

add_test(NAME test_python
         COMMAND ${PYTHON_EXECUTABLE} -m unittest discover
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
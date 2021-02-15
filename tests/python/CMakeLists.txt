pybind11_find_import(numpy REQUIRED)

# Python module for testing
set(PYTHON_PYLENE_SRC ${CMAKE_SOURCE_DIR}/pylene-python/src)
pybind11_add_module(pylena_test pylena_test.cpp
                                ${PYTHON_PYLENE_SRC}/core/image_cast.cpp
                                ${PYTHON_PYLENE_SRC}/core/numpy_format.cpp)
target_link_libraries(pylena_test PRIVATE Pylene)
target_include_directories(pylena_test PRIVATE ${CMAKE_SOURCE_DIR}/pylene-python/include)

# Sanitizer (clang linked statically asan by default unlike gcc)
if ((SANITIZE_ADDRESS OR SANITIZE_UNDEFINED OR SANITIZE_MEMORY OR SANITIZE_THREAD))
    if (CMAKE_CXX_COMPILER_ID STREQUAL Clang)
        target_compile_options(pylena_test PRIVATE -shared-libasan)
        target_link_options(pylena_test PRIVATE -shared-libasan)
    endif()
    add_sanitizers(pylena_test)
    file(COPY asan-pylena-test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set(SANITIZE_SCRIPT asan-pylena-test)
endif()
add_dependencies(build-tests pylena_test)

function(add_python_test filename)
    file(COPY ${filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endfunction(add_python_test)

# PYTHON FILES TO MOVE HERE
add_python_test(test_pylena_numpy.py)

add_test(NAME test_python
        COMMAND ${SANITIZE_SCRIPT} ${PYTHON_EXECUTABLE} -m unittest discover
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(test_python PROPERTIES LABELS UnitTests)
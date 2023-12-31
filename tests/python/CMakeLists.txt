# Copy a script to launch ASAN for python extensions
set(SANITIZE_SCRIPT "")
if (UNIX AND NOT CYGWIN)
    string(FIND "${CMAKE_CXX_FLAGS}" "-fsanitize=address" USE_SANITIZER)
    if (USE_SANITIZER GREATER -1)
        set(SANITIZE_SCRIPT "asan-pylena-test")
        file(COPY ${SANITIZE_SCRIPT} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            add_compile_options("-shared-libasan")
            add_link_options("-shared-libasan")
        endif()
    endif()
endif()

find_package(pybind11 REQUIRED)
include(pybind11Common)

# Python module for testing
pybind11_add_module(pylena_test pylena_test.cpp)
target_link_libraries(pylena_test PRIVATE Pylene Pylene-numpy)
add_dependencies(build-tests pylena_test)

# Dummy extension for testing
pybind11_add_module(pylena_extension extension_pylena_numpy.cpp)
target_link_libraries(pylena_extension PRIVATE Pylene Pylene-numpy)
add_dependencies(build-tests pylena_extension)

function(add_python_test filename)
    file(COPY ${filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endfunction(add_python_test)

# PYTHON FILES TO MOVE HERE
add_python_test(test_pylena_numpy.py)

add_test(NAME test_python
        COMMAND ${SANITIZE_SCRIPT} ${PYTHON_EXECUTABLE} -m unittest discover
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(test_python PROPERTIES LABELS UnitTests)

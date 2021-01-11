function(add_python_test filename)
    file(COPY ${filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endfunction(add_python_test)

# PYTHON FILES TO MOVE HERE
add_python_test(test_pylena_numpy.py)

add_test(NAME test_python
         COMMAND ${PYTHON_EXECUTABLE} -m unittest discover
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/python)
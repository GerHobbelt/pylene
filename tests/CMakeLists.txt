project(tests)
# add link flag to test framework
link_libraries(${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_definitions(-DBOOST_TEST_DYN_LINK)
add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (WIN32)
  add_custom_target(check
    COMMAND ${CMAKE_COMMAND} -E copy "${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${FreeImage_LIBRARIES}" "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_CTEST_COMMAND}
    )
else(WIN32)
  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    )
endif(WIN32)

macro(add_core_test Testname Exename)
add_test(NAME ${Testname} COMMAND ${Exename} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_dependencies(check ${Exename})
endmacro(add_core_test)

message(STATUS "-- *** ${Boost_PROGRAM_OPTIONS_LIBRARIES} ${FreeImage_LIBRARIES}")

add_subdirectory(core)
add_subdirectory(draw)
add_subdirectory(io)
add_subdirectory(colors)
add_subdirectory(morpho)
add_subdirectory(accu)
add_subdirectory(labeling)
add_subdirectory(graph)
add_subdirectory(graphcut)
add_subdirectory(transform)
add_subdirectory(kernelv3)


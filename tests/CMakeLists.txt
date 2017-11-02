project(tests)
# add link flag to test framework
link_libraries(${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_definitions(-DBOOST_TEST_DYN_LINK)
add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (WIN32)
  get_filename_component(UTF_BASE_NAME ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE} NAME_WE)
  get_filename_component(UTF_PATH ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE} PATH)
  get_filename_component(FREEIMAGE_LIB_BASE_NAME ${FreeImage_LIBRARIES} NAME_WE)
  get_filename_component(FREEIMAGE_LIB_PATH ${FreeImage_LIBRARIES} PATH)
  add_custom_target(check
    COMMAND ${CMAKE_COMMAND} -E copy "${UTF_PATH}/${UTF_BASE_NAME}.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
    COMMAND ${CMAKE_COMMAND} -E copy "${FREEIMAGE_LIB_PATH}/${FREEIMAGE_LIB_BASE_NAME}.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
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



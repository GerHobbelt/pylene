project(tests)
# add link flag to test framework
add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (WIN32)
  get_filename_component(FREEIMAGE_LIB_BASE_NAME ${FreeImage_LIBRARIES} NAME_WE)
  get_filename_component(FREEIMAGE_LIB_PATH ${FreeImage_LIBRARIES} PATH)
  add_custom_target(check
    COMMAND ${CMAKE_COMMAND} -E copy "${FREEIMAGE_LIB_PATH}/${FREEIMAGE_LIB_BASE_NAME}.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>"
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    )
else(WIN32)
  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    )
endif(WIN32)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

# Register an executable test
# Executable: The name of the executable (default: ${Prefix}${TestName})
# Source: The source files to compile (default: ${Prefix}${TestName}.cpp)
macro(add_core_test Executable Source)
add_executable(${Executable} ${Source})
add_test(NAME ${Executable} COMMAND ${Executable} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
if (WIN32)
  target_link_libraries(${Executable} ${GTEST_BOTH_LIBRARIES})
else (WIN32)
  target_link_libraries(${Executable} ${GTEST_BOTH_LIBRARIES} pthread)
endif (WIN32)
add_dependencies(check ${Executable})
endmacro(add_core_test)

add_subdirectory(accu)
add_subdirectory(core)
add_subdirectory(draw)
add_subdirectory(io)
add_subdirectory(colors)
add_subdirectory(morpho)
add_subdirectory(labeling)
add_subdirectory(graph)
add_subdirectory(graphcut)
add_subdirectory(transform)

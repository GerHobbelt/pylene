find_package(Doxygen)
find_package(Sphinx)
find_package(Boost COMPONENTS program_options REQUIRED)

add_subdirectory(source/snippets)

if(DOXYGEN_FOUND)
  set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/pylene/include ${PROJECT_SOURCE_DIR}/pylene-python/include")
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
  set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
  file(GLOB_RECURSE PYLENE_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/pylene/include/*.hpp)
  file(GLOB_RECURSE PYLENE_PYTHON_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/pylene-python/*.hpp)
  file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

  add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                    DEPENDS ${PYLENE_PUBLIC_HEADERS} ${PYLENE_PYTHON_PUBLIC_HEADERS}
                    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                    COMMENT "Generating Doxygen documentation")

  add_custom_target(build-doxygen DEPENDS ${DOXYGEN_INDEX_FILE})
  add_dependencies(build-doc build-doxygen)
endif(DOXYGEN_FOUND)


if(SPHINX_FOUND AND DOXYGEN_FOUND)
  set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
  set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
  set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
  file(GLOB_RECURSE SPHINX_SOURCES ${SPHINX_SOURCE} *.rst)
  file(MAKE_DIRECTORY ${SPHINX_BUILD})

  add_custom_target(build-sphinx COMMAND ${SPHINX_EXECUTABLE} -b html
                    -Dbreathe_projects.Pylene=${DOXYGEN_OUTPUT_DIR}/xml
                    ${SPHINX_SOURCE} ${SPHINX_BUILD}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DEPENDS build-images build-doxygen ${SPHINX_SOURCES}
                    COMMENT "Generating Sphinx documentation")

  add_dependencies(build-doc build-sphinx)
endif()

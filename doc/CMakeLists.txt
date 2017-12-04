add_custom_target(doc
  COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
  COMMAND sphinx-build -b html ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/build
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


add_subdirectory(source/snipsets)

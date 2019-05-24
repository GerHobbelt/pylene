add_subdirectory(source/snippets)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doxyxml/index.xml
                   COMMAND doxygen Doxyfile
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(build-doxyxml
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/doxyxml/index.xml)

add_custom_target(doc
                  COMMAND sphinx-build -b html ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/build
                  DEPENDS build-images
                  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/doxyxml/index.xml")

add_dependencies(build-doc doc)

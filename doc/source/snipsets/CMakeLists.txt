##################################
## Gererate All Example Images ###

set(DOCUMENTATION_IMAGE_DIR ${PROJECT_SOURCE_DIR}/doc/source/images)
set(PYLENE_IMAGE_DIR ${PROJECT_SOURCE_DIR}/img)

set(DOCUMENTATION_IMAGES "")
function(add_image EXECUTABLE INPUT OUTPUT)
  set(OUTPUT ${DOCUMENTATION_IMAGE_DIR}/${OUTPUT})
  list(APPEND DOCUMENTATION_IMAGES ${OUTPUT})
  set(DOCUMENTATION_IMAGES ${DOCUMENTATION_IMAGES} PARENT_SCOPE)

  add_custom_command(OUTPUT ${OUTPUT}
    COMMAND ${EXECUTABLE} ${INPUT} ${OUTPUT} ${ARGN}
    COMMAND_EXPAND_LISTS
    WORKING_DIRECTORY ${DOCUMENTATION_IMAGE_DIR})
endfunction()

add_image("erosion-cli;erosion;square;21" ${PYLENE_IMAGE_DIR}/lena.pgm morpho_erosion_1.png)
add_image("erosion-cli;dilation;square;21" ${PYLENE_IMAGE_DIR}/lena.pgm morpho_dilation_1.png)
add_image("erosion-cli;opening;square;21" ${PYLENE_IMAGE_DIR}/lena.pgm morpho_opening_1.png)
add_image("erosion-cli;closing;square;21" ${PYLENE_IMAGE_DIR}/lena.pgm morpho_closing_1.png)
add_image("staff_lines" ${DOCUMENTATION_IMAGE_DIR}/staff_lines.pbm
  morpho_hitormiss_1.png morpho_hitormiss_2.png staff_lines_markers.png morpho_reconstruction_1.png morpho_reconstruction_2.png)


add_custom_target(build-images
  DEPENDS "${DOCUMENTATION_IMAGES}")

link_libraries(${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} ${FreeImage_LIBRARY})

add_executable(erosion-cli erosion-cli.cpp)
add_executable(staff_lines staff_lines.cpp)
add_executable(component_tree_1 component_tree_1.cpp)


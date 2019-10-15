##################################
## Gererate All Example Images ###

set(DOCUMENTATION_IMAGE_DIR ${PROJECT_SOURCE_DIR}/doc/source/images)
set(PYLENE_IMAGE_DIR ${PROJECT_SOURCE_DIR}/img)

set(DOCUMENTATION_IMAGES "")
function(add_image EXECUTABLE INPUT)
  # add_image(EXECUTABLE INPUT OUTPUT1 [ [OUTPUT2 [OUTPUT3 ...] ] ]

  set(outputs "")
  foreach (output IN LISTS ARGN)
    set(output "${DOCUMENTATION_IMAGE_DIR}/${output}")
    list(APPEND outputs "${output}")
    list(APPEND DOCUMENTATION_IMAGES "${output}")
  endforeach ()
  set(DOCUMENTATION_IMAGES ${DOCUMENTATION_IMAGES} PARENT_SCOPE)

  add_custom_command(OUTPUT ${outputs}
                     COMMAND ${EXECUTABLE} ${INPUT} ${outputs}
                     COMMAND_EXPAND_LISTS
                     )
endfunction()

add_image("erosion-cli;erosion;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_erosion_1.png)
add_image("erosion-cli;dilation;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_dilation_1.png)
add_image("erosion-cli;opening;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_opening_1.png)
add_image("erosion-cli;closing;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_closing_1.png)
add_image("erosion-cli;median;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_median_1.png)
add_image("cdt;1" "${DOCUMENTATION_IMAGE_DIR}/F.png" F-4.png)
add_image("cdt;2" "${DOCUMENTATION_IMAGE_DIR}/F.png" F-8.png)
add_image("cdt;3" "${DOCUMENTATION_IMAGE_DIR}/F.png" F-2-3.png)
add_image("cdt;4" "${DOCUMENTATION_IMAGE_DIR}/F.png" F-5-7-11.png)


add_image("staff_lines" "${DOCUMENTATION_IMAGE_DIR}/staff_lines.pbm"
  morpho_hitormiss_1.png morpho_hitormiss_2.png staff_lines_markers.png morpho_reconstruction_1.png morpho_reconstruction_2.png)
add_image("reconstruction"
  "${DOCUMENTATION_IMAGE_DIR}/blobs2_binary.png"
  morpho_reconstruction_dilated.png
  morpho_reconstruction_markers.png
  morpho_reconstruction_rec.png
  morpho_reconstruction_out.png)
add_image("blobs_watershed" "${DOCUMENTATION_IMAGE_DIR}/blobs_binary.png" blobs_distance_transform.png blobs_segmentation.png)


add_custom_target(build-images
                  DEPENDS "${DOCUMENTATION_IMAGES}")

link_libraries(${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} ${FreeImage_LIBRARY})
link_libraries(Pylene::Pylene)

add_executable(erosion-cli erosion-cli.cpp)
add_executable(staff_lines staff_lines.cpp)
add_executable(component_tree_1 component_tree_1.cpp)
add_executable(blobs_watershed blobs_watershed.cpp)
add_executable(reconstruction reconstruction.cpp)
add_executable(cdt cdt.cpp)

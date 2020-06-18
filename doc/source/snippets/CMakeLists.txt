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

add_image("intro-1" "${DOCUMENTATION_IMAGE_DIR}/Olena-c6gradi.png" intro-1-1.png)

add_image("erosion-cli;erosion;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_erosion_1.png)
add_image("erosion-cli;dilation;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_dilation_1.png)
add_image("erosion-cli;opening;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_opening_1.png)
add_image("erosion-cli;closing;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_closing_1.png)
add_image("erosion-cli;median;square;21" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_median_1.png)
add_image("erosion-cli;gradient;square;7" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_gradient_1.png)
add_image("erosion-cli;ext_gradient;square;7" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_ext_gradient_1.png)
add_image("erosion-cli;int_gradient;square;7" "${PYLENE_IMAGE_DIR}/lena.pgm" morpho_int_gradient_1.png)

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
add_image("area_filter"
  "${DOCUMENTATION_IMAGE_DIR}/blobs2_binary.png"
  morpho_area_filter_dilated.png
  morpho_area_filter_opening.png
  morpho_area_filter_out.png)



add_image("blobs_watershed" "${DOCUMENTATION_IMAGE_DIR}/blobs_binary.png" blobs_distance_transform.png blobs_segmentation.png)


add_custom_target(build-images
                  DEPENDS "${DOCUMENTATION_IMAGES}")

add_library(doc-lib lut.cpp)
target_link_libraries(doc-lib Pylene::Pylene)


# for program_options, need to separate CONAN and regular FindBoost
if (TARGET Boost::program_options)
  link_libraries(Boost::program_options)
elseif (TARGET CONAN_LIB::Boost_boost_program_options)
  link_libraries(CONAN_LIB::Boost_boost_program_options)
endif()

link_libraries(Pylene::Pylene)
link_libraries(doc-lib)

add_executable(erosion-cli erosion-cli.cpp)
add_executable(staff_lines staff_lines.cpp)
add_executable(component_tree_1 component_tree_1.cpp)
add_executable(blobs_watershed blobs_watershed.cpp)
add_executable(reconstruction reconstruction.cpp)
add_executable(area_filter area_filter.cpp)
add_executable(cdt cdt.cpp)
add_executable(first_start_1 first_start_1.cpp)
add_executable(intro-1 intro-1.cpp)

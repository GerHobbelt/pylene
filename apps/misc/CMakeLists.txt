link_libraries(${FreeImage_LIBRARIES})

add_executable(print_tos_stat print_tos_stat.cpp $<TARGET_OBJECTS:g2-tools>)
add_executable(reconstruct_from_tree reconstruct_from_tree.cpp)
add_executable(ppm_grain_filter_marginal ppm_grain_filter_marginal.cpp)
add_executable(psnr psnr.cpp)
add_executable(compute_depth compute_depth.cpp)


add_executable(area_close area_close.cpp)
add_executable(grain_filter grain_filter.cpp)
target_link_libraries(area_close -lfreeimage)
target_link_libraries(grain_filter -lfreeimage)

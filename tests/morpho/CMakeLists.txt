set(test_prefix "UTMorpho_")

add_subdirectory(datastruct)
add_subdirectory(maxtree)
add_subdirectory(alphatree)
add_subdirectory(component_tree)

add_core_test(${test_prefix}saturate         saturate.cpp)
add_core_test(${test_prefix}dilate           dilate.cpp)
add_core_test(${test_prefix}erode            erode.cpp)
add_core_test(${test_prefix}gradient         gradient.cpp)
add_core_test(${test_prefix}opening          opening.cpp)
add_core_test(${test_prefix}extinction       extinction.cpp)
add_core_test(${test_prefix}median_filter    median_filter.cpp)
add_core_test(${test_prefix}hit_or_miss      hit_or_miss.cpp)

target_link_libraries(${test_prefix}saturate    ${FreeImage_LIBRARIES})
target_link_libraries(${test_prefix}dilate      ${FreeImage_LIBRARIES})
target_link_libraries(${test_prefix}erode       ${FreeImage_LIBRARIES})
target_link_libraries(${test_prefix}gradient    ${FreeImage_LIBRARIES})
target_link_libraries(${test_prefix}opening     ${FreeImage_LIBRARIES})
target_link_libraries(${test_prefix}median_filter ${FreeImage_LIBRARIES})


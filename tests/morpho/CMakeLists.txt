set(test_prefix "UTMorpho_")

add_core_test(${test_prefix}running_max_1d running_max_1d.cpp)
add_core_test(${test_prefix}block_running_max block_running_max.cpp)
add_core_test(${test_prefix}fill_hole fill_hole.cpp)
add_core_test(${test_prefix}dilate dilate.cpp)
add_core_test(${test_prefix}erode erode.cpp)
add_core_test(${test_prefix}gradient gradient.cpp)
add_core_test(${test_prefix}opening opening.cpp)
add_core_test(${test_prefix}reconstruction reconstruction.cpp)
add_core_test(${test_prefix}extinction extinction.cpp)
add_core_test(${test_prefix}median_filter median_filter.cpp)
add_core_test(${test_prefix}rank_filter rank_filter.cpp)
add_core_test(${test_prefix}hit_or_miss hit_or_miss.cpp)
add_core_test(${test_prefix}watershed watershed.cpp)
add_core_test(${test_prefix}watershed_hierarchy watershed_hierarchy.cpp)
add_core_test(${test_prefix}area_filter area_filter.cpp)
add_core_test(${test_prefix}dynamic_filter dynamic_filter.cpp)
add_core_test(${test_prefix}ToS tos.cpp tos_tests_helper.cpp)

add_core_test(${test_prefix}depth_first depthfirst.cpp)
add_core_test(${test_prefix}maxtree maxtree.cpp)
add_core_test(${test_prefix}alphatree alphatree.cpp)
add_core_test(${test_prefix}private_directional_hqueue directional_hqueue.cpp)
add_core_test(${test_prefix}component_tree component_tree.cpp)
add_core_test(${test_prefix}MToS mtos.cpp)
add_core_test(${test_prefix}waterfall waterfall.cpp)


target_link_libraries(${test_prefix}dilate PRIVATE TBB::tbb)

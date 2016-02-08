set(test_prefix "UTMorphoComponentTree")

add_executable(${test_prefix}component_tree component_tree.cpp)
add_executable(${test_prefix}image_property_map image_property_map.cpp)

add_core_test(${test_prefix}component_tree      ${test_prefix}component_tree)
add_core_test(${test_prefix}image_property_map  ${test_prefix}image_property_map)

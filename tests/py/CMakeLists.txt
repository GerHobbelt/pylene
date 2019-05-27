set(test_prefix "UTPy_")

add_core_test(${test_prefix}value_set   value_set.cpp)
add_core_test(${test_prefix}image2d     image2d.cpp)
add_core_test(${test_prefix}stretch     stretch.cpp)
add_core_test(${test_prefix}any_span    any_span.cpp)
add_core_test(${test_prefix}any_ref     any_ref.cpp)

#add_core_test(${test_prefix}ndbuffer_image ndbuffer_image.cpp)
#target_link_libraries(${test_prefix}ndbuffer_image PRIVATE Pylene-python)

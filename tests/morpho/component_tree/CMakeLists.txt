set(test_prefix "UTMorphoComponentTree_")

add_executable(${test_prefix}io		io.cpp)
add_core_test(${test_prefix}io		${test_prefix}io)
target_link_libraries(${test_prefix}io ${FreeImage_LIBRARIES})
#set_target_properties(io PROPERTIES OUTPUT_NAME maxtree_old)

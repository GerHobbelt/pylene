set(test_prefix "UTMorphoComponentTree_")

add_core_test(${test_prefix}io		io.cpp)
target_link_libraries(${test_prefix}io ${FreeImage_LIBRARIES})

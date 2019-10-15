set(test_prefix "UTLabeling_")

add_core_test(${test_prefix}blobs            blobs.cpp)
add_core_test(${test_prefix}rag              rag.cpp)
add_core_test(${test_prefix}local_extrema    local_extrema.cpp)
add_core_test(${test_prefix}cdt              chamfer_distance_transform.cpp)

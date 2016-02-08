set(test_prefix "UTMorphoComponentTree_")

add_executable(${test_prefix}maxtree_indexes		maxtree.cpp)
add_core_test(${test_prefix}maxtree_indexes		${test_prefix}maxtree_indexes)

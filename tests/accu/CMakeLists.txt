set(test_prefix "UTAccu_")

add_core_test(${test_prefix}sum sum.cpp)
add_core_test(${test_prefix}count count.cpp)
add_core_test(${test_prefix}mean mean.cpp)
add_core_test(${test_prefix}rank rank.cpp)
add_core_test(${test_prefix}infsup infsup.cpp)
add_core_test(${test_prefix}composite_accumulator composite_accumulator.cpp)
add_core_test(${test_prefix}min_elements min_elements.cpp)
add_core_test(${test_prefix}max_elements max_elements.cpp)
add_core_test(${test_prefix}moment_of_inertia moment_of_inertia.cpp)
add_core_test(${test_prefix}cvxhull cvxhull.cpp)

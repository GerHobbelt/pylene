set(test_prefix "UTColors_")

add_executable(${test_prefix}lsh lsh.cpp)
add_core_test(${test_prefix}lsh ${test_prefix}lsh)

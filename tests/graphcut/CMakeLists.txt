set(test_prefix "UTGraphcut_")

add_executable(${test_prefix}graphcut graphcut.cpp)
add_core_test(${test_prefix}graphcut ${test_prefix}graphcut)

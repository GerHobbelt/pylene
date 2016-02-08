set(test_prefix "UTDraw_")

add_executable(${test_prefix}line2d range/line2d.cpp)
add_core_test(${test_prefix}line2d ${test_prefix}line2d)


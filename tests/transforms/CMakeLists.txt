set(test_prefix "UTTransforms_")

add_core_test(${test_prefix}hough_lines hough_lines.cpp)
target_link_libraries(${test_prefix}hough_lines PRIVATE Pylene::Core)

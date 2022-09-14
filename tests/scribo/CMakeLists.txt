set(test_prefix "UTScribo_")



add_core_test(${test_prefix}line_detection line_detection.cpp segdet_tests_helper.cpp)
add_core_test(${test_prefix}lin_regress lin_regress.cpp)

target_link_libraries(${test_prefix}line_detection PRIVATE Pylene::Scribo)
target_link_libraries(${test_prefix}lin_regress PRIVATE Pylene::Scribo)

find_package(Eigen3 3.3.9)

if (Eigen3_FOUND)
    add_core_test(${test_prefix}extraction extraction.cpp)
    target_link_libraries(${test_prefix}extraction PRIVATE Pylene::Scribo Eigen3::Eigen)
endif(Eigen3_FOUND)
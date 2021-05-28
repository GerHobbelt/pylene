set(test_prefix "UTContrib_")

add_executable(meanshift meanshift/meanshift.cpp)
target_link_libraries(meanshift Pylene::Pylene)

add_core_test(${test_prefix}lin_regress              segdet/lin_regress.cpp)

set(test_prefix "UTContrib_")

add_executable(meanshift meanshift/meanshift.cpp)
target_link_libraries(meanshift Pylene::IO-freeimage)
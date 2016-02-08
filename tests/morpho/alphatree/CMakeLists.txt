link_libraries(${FreeImage_LIBRARIES})

add_executable(${test_prefix}alphatree		alphatree.cpp)
add_core_test( ${test_prefix}alphatree          ${test_prefix}alphatree)

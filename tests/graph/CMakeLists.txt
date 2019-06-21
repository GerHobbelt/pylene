set(test_prefix "UTGraph_")

add_core_test(${test_prefix}image2d graph_image2d.cpp)

# FIXME:
# leak sanitizer detect a memory leak here
# temporary workaround
if(SANITIZE_ADDRESS)
  set_tests_properties(${test_prefix}image2d  PROPERTIES WILL_FAIL TRUE)
endif()

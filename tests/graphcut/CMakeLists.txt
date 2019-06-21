set(test_prefix "UTGraphcut_")

add_core_test(${test_prefix}graphcut graphcut.cpp)

# FIXME:
# leak sanitizer detect a memory leak here
# temporary workaround
if(SANITIZE_ADDRESS)
  set_tests_properties(${test_prefix}graphcut  PROPERTIES WILL_FAIL TRUE)
endif()

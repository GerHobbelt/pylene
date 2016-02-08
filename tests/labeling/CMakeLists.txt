set(test_prefix "UTLabeling")

add_executable(${test_prefix}blobs blobs.cpp)
add_executable(${test_prefix}rag   rag.cpp)

add_core_test(${test_prefix}blobs ${test_prefix}blobs)
add_core_test(${test_prefix}rag   ${test_prefix}rag)

set(test_prefix "UTKernel")

include_directories("~/Téléchargements/hana-master/include")

add_executable(${test_prefix}_kernel kernel.cpp)
add_core_test(${test_prefix}_kernel ${test_prefix}_kernel)

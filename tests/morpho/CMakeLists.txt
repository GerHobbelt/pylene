
add_executable(maxtree_ufind maxtree_ufind.cpp)
add_executable(maxtree_hqueue maxtree_hqueue.cpp)
add_executable(maxtree maxtree.cpp)
add_executable(bench_maxtree bench_maxtree.cpp)
target_link_libraries(bench_maxtree ${Boost_PROGRAM_OPTIONS_LIBRARY})



add_test(maxtree_ufind maxtree_ufind)
add_test(maxtree_hqueue maxtree_hqueue)
add_test(maxtree maxtree)

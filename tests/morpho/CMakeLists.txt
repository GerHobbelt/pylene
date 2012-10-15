include_directories(${TBB_INCLUDE_DIRS})
link_directories(${TBB_LIBRARY_DIRS})

add_executable(maxtree_ufind maxtree_ufind.cpp)
add_executable(maxtree_hqueue maxtree_hqueue.cpp)
add_executable(maxtree maxtree.cpp)

add_test(maxtree_ufind maxtree_ufind)
add_test(maxtree_hqueue maxtree_hqueue)
add_test(maxtree maxtree)

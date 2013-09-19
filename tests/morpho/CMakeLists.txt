add_executable(maxtree		maxtree.cpp)
add_core_test(maxtree		maxtree)

add_executable(saturate	saturate.cpp)
target_link_libraries(saturate -lfreeimage)
add_core_test(saturate saturate)

add_executable(dilate	dilate.cpp)
target_link_libraries(dilate -lfreeimage)
add_core_test(dilate dilate)

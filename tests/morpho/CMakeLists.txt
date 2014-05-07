add_subdirectory(datastruct)
add_subdirectory(maxtree)
add_subdirectory(component_tree)

add_executable(maxtree		maxtree.cpp)
add_core_test(maxtree		maxtree)
set_target_properties(maxtree PROPERTIES OUTPUT_NAME maxtree_old)

add_executable(saturate         saturate.cpp)
add_executable(dilate           dilate.cpp)
add_executable(extinction       extinction.cpp)

target_link_libraries(saturate -lfreeimage)
target_link_libraries(dilate -lfreeimage)

add_core_test(saturate          saturate)
add_core_test(dilate            dilate)
add_core_test(extinction        extinction)

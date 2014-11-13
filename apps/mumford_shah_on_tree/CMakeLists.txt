link_libraries(-lfreeimage)

add_executable(mumford_shah_on_tree mumford_shah_on_tree.cpp)
add_executable(mumford_shah_on_tree_full mumford_shah_on_tree_full.cpp
  ${CMAKE_SOURCE_DIR}/apps/g2/satmaxtree.cpp
  ${CMAKE_SOURCE_DIR}/apps/g2/compute_g2.cpp)

target_link_libraries(mumford_shah_on_tree_full ${TBB_LIBRARIES})
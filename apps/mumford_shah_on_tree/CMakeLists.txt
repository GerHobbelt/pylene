link_libraries(${FreeImage_LIBRARIES} Pylene::Pylene)
include_directories(${PROJECT_SOURCE_DIR}/..)

add_executable(mumford_shah_on_tree mumford_shah_on_tree.cpp)
add_executable(mumford_shah_on_tree_16 mumford_shah_on_tree.cpp)

add_executable(mumford_shah_on_tree_full mumford_shah_on_tree_full.cpp
  ${CMAKE_SOURCE_DIR}/apps/g2/satmaxtree.cpp
  ${CMAKE_SOURCE_DIR}/apps/g2/compute_g2.cpp)

set_target_properties(mumford_shah_on_tree_16 PROPERTIES COMPILE_DEFINITIONS MLN_INPUT_VALUE_TYPE=mln::rgb16)

target_link_libraries(mumford_shah_on_tree_full tbb)

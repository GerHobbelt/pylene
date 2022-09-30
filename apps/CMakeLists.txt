project(apps)

link_libraries(Pylene)
include_directories(${CMAKE_SOURCE_DIR})


add_subdirectory(maxtree_comparison)

add_subdirectory(attributes)
add_subdirectory(g2)
add_subdirectory(mumford_shah_on_tree)
add_subdirectory(misc)
add_subdirectory(theo)
add_subdirectory(tosgui)
add_subdirectory(hierachical_seg-gui)
add_subdirectory(hyperspec)
add_subdirectory(icip.2022)
add_subdirectory(tip.2019-2020)
add_subdirectory(gpce.2022)

## Old apps no longer supported or using an old API
# add_subdirectory(simplification)
# add_subdirectory(clattice)
# add_subdirectory(saliency)

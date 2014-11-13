link_libraries(-lfreeimage)

add_executable(g2-maxdepth g2-maxdepth.cpp routines.cpp compute_g2.cpp)
add_executable(satmaxtree-cli satmaxtree-cli.cpp satmaxtree.cpp)

target_link_libraries(g2-maxdepth ${TBB_LIBRARIES})

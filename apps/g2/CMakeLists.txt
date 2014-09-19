link_libraries(${FreeImage_LIBRARIES})

add_executable(g2-maxdepth g2-maxdepth.cpp routines.cpp compute_g2.cpp)
add_executable(satmaxtree-cli satmaxtree-cli.cpp satmaxtree.cpp)

link_libraries(${FreeImage_LIBRARIES})


add_library(g2-tools OBJECT satmaxtree.cpp compute_ctos.cpp routines.cpp compute_g2.cpp)
add_executable(g2-maxdepth g2-maxdepth.cpp $<TARGET_OBJECTS:g2-tools>)
add_executable(g2-maxtree g2-maxtree.cpp $<TARGET_OBJECTS:g2-tools>)
add_executable(compute_ctos-cli  compute_ctos-cli.cpp $<TARGET_OBJECTS:g2-tools>)

add_library(g2-tools-16 OBJECT routines.cpp compute_g2.cpp)
add_executable(g2-maxdepth-16 g2-maxdepth.cpp $<TARGET_OBJECTS:g2-tools-16>)
add_executable(g2-maxtree-16 g2-maxtree.cpp $<TARGET_OBJECTS:g2-tools-16>)

set_target_properties(g2-tools-16 g2-maxdepth-16 g2-maxtree-16 PROPERTIES COMPILE_DEFINITIONS MLN_INPUT_VALUE_TYPE=mln::rgb16)

add_executable(satmaxtree-cli satmaxtree-cli.cpp $<TARGET_OBJECTS:g2-tools>)

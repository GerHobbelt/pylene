link_libraries(-lfreeimage)
add_executable(simplify_gray simplify_gray.cpp)
add_executable(simplify_color simplify_color.cpp ../tos/colorToSGrad.cpp)




link_libraries(${FreeImage_LIBRARIES})

add_executable(alphatree		alphatree.cpp)
add_core_test(alphatree                 alphatree)

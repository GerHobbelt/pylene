add_executable(gen_figs gen_figs.cpp)
target_link_libraries(gen_figs PRIVATE Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene ${FreeImage_LIBRARIES})

add_dependencies(build-apps gen_figs)

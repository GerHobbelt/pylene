add_executable(gen_figs gen_figs.cpp)
target_link_libraries(gen_figs PRIVATE Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene ${FreeImage_LIBRARIES})

add_executable(blur blur.cpp)
target_link_libraries(blur PRIVATE Pylene::Pylene)


add_sanitizers(gen_figs)

add_dependencies(build-apps gen_figs)

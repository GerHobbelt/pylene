find_package(fmt 7.0 REQUIRED)

add_executable(gen_figs gen_figs.cpp)
target_link_libraries(gen_figs PRIVATE fmt::fmt Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene ${FreeImage_LIBRARIES})
target_compile_features(gen_figs PRIVATE cxx_std_20)

add_executable(blur blur.cpp)
target_link_libraries(blur PRIVATE Pylene::Pylene)
target_compile_features(blur PRIVATE cxx_std_20)


add_sanitizers(gen_figs)

add_dependencies(build-apps gen_figs)

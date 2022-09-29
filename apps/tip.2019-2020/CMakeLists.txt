find_package(fmt 6.0 REQUIRED)

add_executable(gen_figs gen_figs.cpp)
target_link_libraries(gen_figs PRIVATE fmt::fmt Fixtures::ImagePath Fixtures::ImageCompare Pylene::Pylene Pylene::IO-freeimage)
target_compile_features(gen_figs PRIVATE cxx_std_20)

add_executable(blur blur.cpp)
target_link_libraries(blur PRIVATE Pylene::Pylene Pylene::IO-freeimage)
target_compile_features(blur PRIVATE cxx_std_20)

add_executable(print_se print_se.cpp)
target_link_libraries(print_se PRIVATE Pylene::Pylene fmt::fmt Pylene::IO-freeimage)
target_compile_features(print_se PRIVATE cxx_std_20)


# add_sanitizers(gen_figs)

add_dependencies(build-apps gen_figs)

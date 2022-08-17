find_package(fmt 6.0 REQUIRED)

add_executable(labelize labelize.cpp)
target_link_libraries(labelize PRIVATE fmt::fmt Fixtures::ImagePath Fixtures::ImageCompare Anonymous::Anonymous Anonymous::IO-freeimage)
target_compile_features(labelize PRIVATE cxx_std_20)


add_dependencies(build-apps labelize)

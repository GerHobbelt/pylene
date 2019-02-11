add_subdirectory(ImagePath)
add_subdirectory(ImageCompare)

add_library(Fixtures::ImagePath ALIAS ImagePath)
add_library(Fixtures::ImageCompare ALIAS ImageCompare)

add_dependencies(build-fixtures Fixtures::ImagePath Fixtures::ImageCompare)

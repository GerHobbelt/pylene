add_subdirectory(ImagePath)
add_subdirectory(Neighborhood)

add_library(Fixtures::ImagePath ALIAS ImagePath)
add_library(Fixtures::Neighborhood ALIAS Neighborhood)

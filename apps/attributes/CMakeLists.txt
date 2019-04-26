link_libraries(${FreeImage_LIBRARIES})
find_package(Eigen3)

add_executable(stat stat.cpp)

add_executable(MSER-cli MSER-cli.cpp attributes.cpp)
add_executable(meaningfullness-cli meaningfullness-cli.cpp curvature.cpp attributes.cpp)

target_link_libraries(MSER-cli ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(meaningfullness-cli ${Boost_PROGRAM_OPTIONS_LIBRARY})

if (EIGEN3_FOUND)
  add_executable(circularity-cli circularity-cli.cpp circularity.cpp attributes.cpp)
  target_include_directories(circularity-cli PRIVATE ${EIGEN3_INCLUDE_DIR})
  target_link_libraries(circularity-cli PUBLIC ${Boost_PROGRAM_OPTIONS_LIBRARY})
endif (EIGEN3_FOUND)

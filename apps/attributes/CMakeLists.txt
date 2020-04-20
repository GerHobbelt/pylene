link_libraries(${FreeImage_LIBRARIES})

add_executable(MSER-patternspec-cli MSER-patternspec-cli.cpp)
add_executable(meaningfullness-patternspec-cli meaningfullness-patternspec-cli.cpp curvature.cpp)
add_executable(meaningfullness-cli meaningfullness-cli.cpp curvature.cpp)
add_executable(stat stat.cpp)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

if (WIN32)
  link_libraries(${GTEST_BOTH_LIBRARIES})
else (WIN32)
  link_libraries(${GTEST_BOTH_LIBRARIES} pthread)
endif (WIN32)

add_subdirectory(tos)
add_subdirectory(attributes)

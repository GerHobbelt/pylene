add_custom_target(
  check-internals
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --schedule-random --output-on-failure
)

add_subdirectory(Neighborhood)

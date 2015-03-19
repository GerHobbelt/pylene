add_executable(getquad-cli getquad-cli.cpp getquad.cpp)
target_link_libraries(getquad-cli ${FreeImage_LIBRARIES})
set_property(TARGET getquad-cli APPEND PROPERTY
  INCLUDE_DIRECTORIES /lrde-work/groningen/carlinet/src/eigen/)
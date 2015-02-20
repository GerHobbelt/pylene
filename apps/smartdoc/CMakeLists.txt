add_executable(smartdoc_app main.cpp
  video_tools.cpp
  smartdoc.cpp
  export.cpp
  $<TARGET_OBJECTS:g2-tools>
  )

add_executable(smartdoc_app2 main2.cpp
  video_tools.cpp
  smartdoc.cpp
  export.cpp
  $<TARGET_OBJECTS:g2-tools>
  )

add_executable(smartdoc_cli smartdoc-cli.cpp smartdoc.cpp)

add_executable(challenge2 challenge2.cpp unhomo.cpp border_detection.cpp)

target_link_libraries(smartdoc_cli ${FreeImage_LIBRARIES})

target_link_libraries(smartdoc_app
  -lavcodec -lavformat -lswscale
  ${FreeImage_LIBRARIES})

target_link_libraries(smartdoc_app2
  -lavcodec -lavformat -lswscale -lboost_date_time
  ${FreeImage_LIBRARIES})

target_link_libraries(challenge2 ${FreeImage_LIBRARIES})

set_source_files_properties(video_tools.cpp PROPERTIES
  COMPILE_DEFINITIONS __STDC_CONSTANT_MACROS)

set_property(TARGET challenge2 APPEND PROPERTY
  INCLUDE_DIRECTORIES /work/carlinet/src/eigen/)
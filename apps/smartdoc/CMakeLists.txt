add_executable(smartdoc_app main.cpp
  video_tools.cpp
  smartdoc.cpp
  export.cpp
  ../g2/compute_g2.cpp
  ../g2/compute_ctos.cpp
  ../g2/satmaxtree.cpp
  )

add_executable(smartdoc_cli smartdoc-cli.cpp smartdoc.cpp)

target_link_libraries(smartdoc_cli ${FreeImage_LIBRARIES})

target_link_libraries(smartdoc_app
  -lavcodec -lavformat -lswscale
  ${FreeImage_LIBRARIES})

set_source_files_properties(video_tools.cpp PROPERTIES
  COMPILE_DEFINITIONS __STDC_CONSTANT_MACROS)

#target_set_properties(
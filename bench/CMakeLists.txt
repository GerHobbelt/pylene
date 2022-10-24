project(bench)

find_package(benchmark REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ExternalData)
include(BenchmarkMacros)

# Download file dependency
set(ExternalData_URL_TEMPLATES
  "file:///lrde/dload/olena/pylene/data/%(algo)/%(hash)"
  "https://www.lrde.epita.fr/dload/olena/pylene/data/%(algo)/%(hash)")

ExternalData_Expand_Arguments(
  fetch-external-data
  images
  DATA{Space1_20MB.jpg}
  DATA{Aerial_view_of_Olbia.jpg}
  )

# Extra compiler options

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -g")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-g -fopt-info-vec -fopt-info-vec-missed -ftree-vectorize -ftree-vectorizer-verbose=2")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -g")
else ()
  # W4141: 'inline' : utilisé plusieurs fois in benchmark.h
  set(STANDALONE_COMPILE_FLAGS " ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4141")
endif ()

add_library(BenchImpl
  include/bench/neighborhood.hpp
  src/neighborhood.cpp)


set_target_properties(BenchImpl PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
target_include_directories(BenchImpl PUBLIC include)
target_link_libraries(BenchImpl PUBLIC Pylene::Core Pylene::IO-freeimage)


add_subdirectory(tests)

set(src_standalone
  BMErosion.cpp
  BMMorphers.cpp
  BMNeighborhood.cpp
  BMReference_Neighborhood.cpp
  BMReference_Linear.cpp
  BMAlgorithms.cpp
  )

set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})


add_benchmark(BMAlgorithms              BMAlgorithms.cpp BMAlgorithms_main.cpp)
add_benchmark(BMMorphoMaxtree           BMMorphoMaxtree.cpp)
add_benchmark(BMMorphoTos               BMMorphoTos.cpp)
add_benchmark(BMMorphoBase              BMMorphoBase.cpp)
add_benchmark(BMMorphers                BMMorphers.cpp BMMorphers_main.cpp)
add_benchmark(BMReference_Linear        BMReference_Linear.cpp BMReference_Linear_Reversed.cpp BMReference_Linear_main.cpp)
add_benchmark(BMReference_Neighborhood  BMReference_Neighborhood_main.cpp)
add_benchmark(BMBufferPrimitives        BMBufferPrimitives.cpp)
add_benchmark(BMAlphaTree               BMAlphaTree.cpp)
add_benchmark(BMWatershedHierarchy      BMWatershedHierarchy.cpp)
add_benchmark(BM_gpce2022_rebutal       BM_gpce2022_rebutal.cpp)

set(OpenCV_STATIC ON)
set(OpenCV_SHARED OFF)
find_package(OpenCV)

if (OpenCV_FOUND)
  message(STATUS "${OpenCV_LIBS}")
  get_target_property(target_type opencv_core TYPE)
  if (target_type STREQUAL STATIC_LIBRARY)
    message(STATUS "opencv_core static")
  endif()
  if (target_type STREQUAL SHARED_LIBRARY)
    message(STATUS "opencv_core shared")
  endif()
  #opencv_calib3d
  #opencv_dnn
  #opencv_features2d
  #opencv_flann
  #opencv_highgui
  #opencv_imgcodecs
  #opencv_imgproc
  #opencv_ml
  #opencv_objdetect
  #opencv_photo
  #opencv_stitching
  #opencv_video
  #opencv_videoio
  #opencv_alphamat
  #opencv_aruco
  #opencv_barcode
  #opencv_bgsegm
  #opencv_bioinspired
  #opencv_ccalib
  #opencv_cvv
  #opencv_datasets
  #opencv_dnn_objdetect
  #opencv_dnn_superres
  #opencv_dpm
  #opencv_face
  #opencv_freetype
  #opencv_fuzzy
  #opencv_hdf
  #opencv_hfs
  #opencv_img_hash
  #opencv_intensity_transform
  #opencv_line_descriptor
  #opencv_mcc
  #opencv_optflow
  #opencv_phase_unwrapping
  #opencv_plot
  #opencv_quality
  #opencv_rapid
  #opencv_reg
  #opencv_rgbd
  #opencv_saliency
  #opencv_shape
  #opencv_stereo
  #opencv_structured_light
  #opencv_superres
  #opencv_surface_matching
  #opencv_text
  #opencv_tracking
  #opencv_videostab
  #opencv_viz
  #opencv_wechat_qrcode
  #opencv_ximgproc
  #opencv_xobjdetect
  #opencv_xphoto

  add_library(BenchImpl_bgSub_views SHARED include/bench/bg_sub.hpp src/bg_sub_views.cpp)
  set_property(TARGET BenchImpl_bgSub_views PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(BenchImpl_bgSub_views PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
  target_include_directories(BenchImpl_bgSub_views PUBLIC include)
  target_link_libraries(BenchImpl_bgSub_views PUBLIC Pylene::Pylene Pylene::IO-freeimage)

  add_library(BenchImpl_bgSub_algos SHARED include/bench/bg_sub.hpp src/bg_sub_algos.cpp)
  set_property(TARGET BenchImpl_bgSub_algos PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(BenchImpl_bgSub_algos PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
  target_include_directories(BenchImpl_bgSub_algos PUBLIC include)
  target_link_libraries(BenchImpl_bgSub_algos PUBLIC Pylene::Pylene Pylene::IO-freeimage)

  add_library(BenchImpl_bgSub_opencv SHARED include/bench/bg_sub.hpp src/bg_sub_opencv.cpp)
  set_property(TARGET BenchImpl_bgSub_opencv PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(BenchImpl_bgSub_opencv PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
  target_include_directories(BenchImpl_bgSub_opencv PUBLIC include)
  #target_link_libraries(BenchImpl_bgSub_opencv PUBLIC Pylene::Pylene ${OpenCV_LIBS})
  target_link_libraries(BenchImpl_bgSub_opencv PUBLIC Pylene::Pylene)

  target_include_directories(BenchImpl_bgSub_opencv PUBLIC "/usr/include/opencv4/")
  target_link_libraries(BenchImpl_bgSub_opencv PUBLIC "libopencv_core.a")

  add_library(BenchImplOpenCV include/bench/bg_sub.hpp src/bg_sub.cpp)
  set_target_properties(BenchImplOpenCV PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})
  target_include_directories(BenchImplOpenCV PUBLIC include)
  target_link_libraries(BenchImplOpenCV PUBLIC Pylene::Pylene Pylene::IO-freeimage ${OpenCV_LIBS})
  

  set(src_standalone_opencv
    opencv/BMPlnVsOpenCV_dilation.cpp
    opencv/BMPlnVsOpenCV_bgSubPipeline.cpp
  )

  #set_source_files_properties(${src_standalone_opencv} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})

  add_benchmark(BMPlnVsOpenCV_Dilation          opencv/BMPlnVsOpenCV_dilation.cpp)
  target_link_libraries(BMPlnVsOpenCV_Dilation  PRIVATE ${OpenCV_LIBS} BenchImplOpenCV)

  add_benchmark(BMPlnVsOpenCV_BgSubPipeline         opencv/BMPlnVsOpenCV_bgSubPipeline.cpp)
  target_link_libraries(BMPlnVsOpenCV_BgSubPipeline PRIVATE ${OpenCV_LIBS} BenchImplOpenCV)

else()
  message("OpenCV not found: not building BMPlnVsOpenCV_Dilation nor BMPlnVsOpenCV_bgSubPipeline")
endif()


ExternalData_Add_Target(fetch-external-data)

include(CMakePackageConfigHelpers)

find_package(Boost 1.58 REQUIRED)
find_package(FreeImage REQUIRED)

find_package(TBB REQUIRED tbb)
find_package(range-v3 0.10.0 REQUIRED)
find_package(fmt 6.0 REQUIRED)
find_package(xsimd REQUIRED)
find_package(Eigen3 3.3.9)
find_package(cfitsio)

set(PYLENE_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")


add_library(Pylene-core)

if (TBB_FOUND AND PYLENE_USE_TBB)
  target_link_libraries(Pylene-core PRIVATE tbb::tbb)
else ()
  set(PYLENE_USE_TBB OFF)
  target_compile_definitions(Pylene-core PRIVATE MLN_NO_TBB)
endif ()

# FIXME: still useful ?
if (UNIX AND NOT APPLE)
  target_link_libraries(Pylene-core INTERFACE rt)
elseif (MSVC)
  #
  target_compile_options(Pylene-core PUBLIC
    "/wd4275" # non dll-interface class '...' used as base for dll-interface (fmt lib)
    "/wd5104" # found 'L#x' in macro replacement list, did you mean
    "/Zc:preprocessor"  # required by range-v3
    "/permissive-" # required by range-v3
    )
endif ()


# Set dependancies
target_include_directories(Pylene-core PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(Pylene-core SYSTEM PUBLIC $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
target_link_libraries(Pylene-core PUBLIC range-v3::range-v3 xsimd)
target_link_libraries(Pylene-core PUBLIC fmt::fmt)

# Set sources
#file(GLOB_RECURSE sources "include/mln/*.hpp")
#target_sources(Pylene INTERFACE $<BUILD_INTERFACE:${sources}>)

add_library(Pylene-bp OBJECT
  src/bp/transpose.cpp
  src/bp/swap.cpp
  src/bp/alloc.cpp
  )

target_include_directories(Pylene-bp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Pylene-bp PRIVATE xsimd fmt::fmt)
target_compile_features(Pylene-bp PUBLIC cxx_std_20)

target_link_libraries(Pylene-core PRIVATE Pylene-bp)


target_sources(Pylene-core PRIVATE
               src/accu/cvxhull.cpp
               src/core/image_format.cpp
               src/core/init_list.cpp
               src/core/ndbuffer_image.cpp
               src/core/ndbuffer_image_data.cpp
               src/core/value_set.cpp
               src/core/padding.cpp
               src/core/parallel_local.cpp
               src/core/parallel_pointwise.cpp
               src/core/se/disc.cpp
               src/core/se/mask2d.cpp
               src/core/se/periodic_line2d.cpp
               src/core/se/rect2d.cpp
               src/core/trace.cpp
               src/core/traverse2d.cpp
               src/io/imprint.cpp
               src/morpho/block_running_max.cpp
               src/morpho/component_tree.cpp
               src/morpho/filters2d.cpp
               src/morpho/hvector.cpp
               src/morpho/hvector_unbounded.cpp
               src/morpho/running_max_vertical2d.cpp
               src/morpho/gaussian2d.cpp
               src/morpho/gaussian2d_coefs.cpp
               src/morpho/immersion.cpp
               src/morpho/maxtree.cpp
               src/morpho/mtos.cpp
               src/morpho/satmaxtree.cpp
               src/morpho/trees_fusion.cpp
               src/morpho/unionfind.cpp
               src/morpho/waterfall.cpp
               src/transforms/hough_lines.cpp
               )

add_library(Pylene-io-freeimage)
add_library(Pylene::IO-freeimage ALIAS Pylene-io-freeimage)
target_sources(Pylene-io-freeimage PRIVATE
               src/io/freeimage_plugin.cpp
               src/io/imread.cpp
               src/io/io.cpp            
)
target_include_directories(Pylene-io-freeimage PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>
)
target_link_libraries(Pylene-io-freeimage PUBLIC Pylene-core)
#target_link_libraries(Pylene-io-freeimage PRIVATE freeimage::freeimage)
target_link_libraries(Pylene-io-freeimage PRIVATE ${FreeImage_LIBRARIES})

#if (cfitsio_FOUND)
#  add_library(Pylene-io-fits)
#  add_library(Pylene::IO-fits ALIAS Pylene-io-fits)
#  target_sources(Pylene-io-fits PRIVATE
#                 src/io/cfitsio_plugin.cpp
#                 src/io/io.cpp
#  )
#  target_include_directories(Pylene-io-fits PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#                             $<INSTALL_INTERFACE:include>
#  )
#  target_link_libraries(Pylene-io-fits PUBLIC Pylene-core)
#  target_link_libraries(Pylene-io-fits PRIVATE cfitsio::cfitsio)
#endif(cfitsio_FOUND)

if (Eigen3_FOUND)
  add_library(Pylene-scribo)
  add_library(Pylene::Scribo ALIAS Pylene-scribo)

  target_include_directories(Pylene-scribo PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                             $<INSTALL_INTERFACE:include>)

  target_sources(Pylene-scribo PRIVATE
          src/scribo/segdet/filter.cpp
          src/scribo/segdet/label_array.cpp
          src/scribo/segdet/linearregression.cpp
          src/scribo/segdet/parameters.cpp
          src/scribo/segdet/preprocess.cpp
          src/scribo/segdet/process.cpp
          src/scribo/segdet/postprocess.cpp
          src/scribo/segdet/segdet.cpp
          src/scribo/segdet/segment.cpp
  )

  target_link_libraries(Pylene-scribo PUBLIC Pylene-core)
  target_link_libraries(Pylene-scribo PRIVATE Eigen3::Eigen)
endif(Eigen3_FOUND)

# Compiler configurations
target_compile_features(Pylene-core PUBLIC cxx_std_20)

if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
  target_compile_options(Pylene-bp PUBLIC -fconcepts)
  target_compile_options(Pylene-core PUBLIC -fconcepts)
  target_compile_options(Pylene-io-freeimage PUBLIC -fconcepts)
endif()

if (PYLENE_BUILD_PYTHON)
  set_property(TARGET Pylene-core PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()

# IDE configuration
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/mln FILES ${sources})


add_library(Pylene::Core ALIAS Pylene-core)


# GCC8 has enoying random compilation failures
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_compile_definitions(Pylene-core PUBLIC PYLENE_GCC8_WORKAROUND)
endif()

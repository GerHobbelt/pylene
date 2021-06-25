include(CMakePackageConfigHelpers)

find_package(Boost 1.58 REQUIRED)
find_package(FreeImage REQUIRED)

find_package(TBB REQUIRED tbb)
find_package(range-v3 0.10.0 REQUIRED)
find_package(fmt 6.0 REQUIRED)
find_package(xsimd REQUIRED)

set(PYLENE_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")


add_library(Pylene-core)

if (TBB_FOUND AND PYLENE_USE_TBB)
  target_link_libraries(Pylene-core PRIVATE TBB::TBB)
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
    "/experimental:preprocessor"  # required by range-v3
    "/permissive-" # required by range-v3
    )
endif ()


# Set dependancies
target_include_directories(Pylene-core PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(Pylene-core SYSTEM PUBLIC $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
target_link_libraries(Pylene-core PUBLIC range-v3::range-v3 xsimd::xsimd)
target_link_libraries(Pylene-core PUBLIC fmt::fmt)
target_link_libraries(Pylene-core PRIVATE FreeImage::FreeImage)


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
target_link_libraries(Pylene-bp PRIVATE xsimd::xsimd fmt::fmt)
target_compile_features(Pylene-bp PUBLIC cxx_std_20)

target_link_libraries(Pylene-core PRIVATE Pylene-bp)


target_sources(Pylene-core PRIVATE
               src/accu/cvxhull.cpp
               src/core/image_format.cpp
               src/core/init_list.cpp
               src/core/ndbuffer_image.cpp
               src/core/ndbuffer_image_data.cpp
               src/core/padding.cpp
               src/core/parallel_local.cpp
               src/core/parallel_pointwise.cpp
               src/core/se/disc.cpp
               src/core/se/mask2d.cpp
               src/core/se/periodic_line2d.cpp
               src/core/se/rect2d.cpp
               src/core/trace.cpp
               src/core/traverse2d.cpp
               src/io/freeimage_plugin.cpp
               src/io/imprint.cpp
               src/io/imread.cpp
               src/io/io.cpp
               src/morpho/block_running_max.cpp
               src/morpho/component_tree.cpp
               src/morpho/hvector.cpp
               src/morpho/hvector_unbounded.cpp
               src/morpho/immersion.cpp
               src/morpho/maxtree.cpp
               src/morpho/unionfind.cpp
               src/morpho/filters2d.cpp
               )

# Compiler configurations
target_compile_features(Pylene-core PUBLIC cxx_std_20)

if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
  target_compile_options(Pylene-bp PUBLIC -fconcepts)
  target_compile_options(Pylene-core PUBLIC -fconcepts)
endif()

# IDE configuration
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/mln FILES ${sources})


add_library(Pylene::Core ALIAS Pylene-core)


# GCC8 has enoying random compilation failures
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_compile_definitions(Pylene-core PUBLIC PYLENE_GCC8_WORKAROUND)
endif()

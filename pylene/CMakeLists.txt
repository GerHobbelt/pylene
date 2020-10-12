include(CMakePackageConfigHelpers)

find_package(Boost 1.58 REQUIRED)
find_package(FreeImage REQUIRED)

find_package(TBB REQUIRED tbb)
find_package(range-v3 0.10.0 REQUIRED)
find_package(fmt 6.0 REQUIRED)
find_package(xsimd REQUIRED)

set(PYLENE_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")


add_library(Pylene)

if (TBB_FOUND AND PYLENE_USE_TBB)
  target_link_libraries(Pylene PRIVATE TBB::TBB)
else ()
  set(PYLENE_USE_TBB OFF)
  target_compile_definitions(Pylene PRIVATE MLN_NO_TBB)
endif ()

# FIXME: still useful ?
if (UNIX AND NOT APPLE)
  target_link_libraries(Pylene INTERFACE rt)
elseif (MSVC)
  #
  target_compile_options(Pylene PUBLIC
    "/wd4275" # non dll-interface class '...' used as base for dll-interface (fmt lib)
    "/wd5104" # found 'L#x' in macro replacement list, did you mean
    "/experimental:preprocessor"  # required by range-v3
    "/permissive-" # required by range-v3
    )
endif ()


# Set dependancies
target_include_directories(Pylene PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(Pylene SYSTEM PUBLIC $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
target_link_libraries(Pylene PUBLIC range-v3::range-v3 xsimd::xsimd)
target_link_libraries(Pylene PUBLIC fmt::fmt)
target_link_libraries(Pylene PRIVATE FreeImage::FreeImage)


# Set sources
#file(GLOB_RECURSE sources "include/mln/*.hpp")
#target_sources(Pylene INTERFACE $<BUILD_INTERFACE:${sources}>)

target_sources(Pylene PRIVATE
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
               )

# Compiler configurations
target_compile_features(Pylene PUBLIC cxx_std_20)

if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
  target_compile_options(Pylene PUBLIC -fconcepts)
endif()

# IDE configuration
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/mln FILES ${sources})


add_library(Pylene::Pylene ALIAS Pylene)

include(GNUInstallDirs)
install(TARGETS Pylene
        EXPORT PyleneTargets)

write_basic_package_version_file(
    PyleneConfigVersion.cmake
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    PyleneConfig.cmake.in
    PyleneConfig.cmake
    @ONLY
)

install(EXPORT PyleneTargets
        FILE PyleneTargets.cmake
        NAMESPACE Pylene::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pylene
        )

install(DIRECTORY include/mln
        TYPE INCLUDE)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/PyleneConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/PyleneConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pylene)

# GCC8 has enoying random compilation failures
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_compile_definitions(Pylene PUBLIC PYLENE_GCC8_WORKAROUND)
endif()

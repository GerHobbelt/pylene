include(CMakePackageConfigHelpers)

find_package(Boost 1.58 REQUIRED)
find_package(range-v3 0.4.0 REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)

set(PYLENE_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")


add_library(Pylene)
add_library(Pylene-python)

if (TBB_FOUND AND PYLENE_USE_TBB)
  target_link_libraries(Pylene INTERFACE tbb)
else()
  set(PYLENE_USE_TBB OFF)
  #FIXME use a config file
  target_compile_definitions(Pylene INTERFACE MLN_NO_TBB)
endif()

# FIXME: still useful ?
if (UNIX AND NOT APPLE)
   target_link_libraries(Pylene INTERFACE rt)
endif()


# Set dependancies
target_include_directories(Pylene PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)

target_include_directories(Pylene SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(Pylene PUBLIC ${FreeImage_INCLUDE_DIRS})
set_property(TARGET Pylene APPEND
  PROPERTY INTERFACE_LINK_DIRECTORIES ${FreeImage_LIBRARY_DIR} ${Boost_LIBRARY_DIRS})
target_link_libraries(Pylene PUBLIC range-v3)


# Set sources
file(GLOB_RECURSE sources "include/mln/*.hpp")
target_sources(Pylene INTERFACE $<BUILD_INTERFACE:${sources}>)

target_sources(Pylene PRIVATE
  src/core/se/disc.cpp
  src/core/se/periodic_line2d.cpp
  src/core/se/rect2d.cpp
  src/core/trace.cpp

  src/py/algorithm/stretch.cpp
  src/py/core/image2d_data.cpp
  )

target_sources(Pylene-python PRIVATE
  src/py/core/image_format.cpp
  src/py/core/ndbuffer_image_data.cpp
  src/py/core/ndbuffer_image.cpp
  )

target_include_directories(Pylene-python PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)


# Compiler configurations
target_compile_features(Pylene PUBLIC cxx_std_17)
target_compile_features(Pylene-python PUBLIC cxx_std_17)

# Add concepts support for gcc > 7.2 with -fconcepts
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.2)
  target_compile_options(Pylene PUBLIC -fconcepts)
  find_package(cmcstl2 REQUIRED)
  target_link_libraries(Pylene PUBLIC stl2)
  target_compile_definitions(Pylene PUBLIC PYLENE_CONCEPT_TS_ENABLED)
endif()



# IDE configuration
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/mln FILES ${sources})



add_library(Pylene::Pylene ALIAS Pylene)
install(TARGETS Pylene
  EXPORT PyleneTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include)

write_basic_package_version_file(
    PyleneConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
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
  DESTINATION lib/cmake/pylene
  )
install(DIRECTORY include/mln
  DESTINATION include)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/PyleneConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/PyleneConfigVersion.cmake
  DESTINATION lib/cmake/pylene )

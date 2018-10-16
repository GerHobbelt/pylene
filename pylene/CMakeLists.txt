include(CMakePackageConfigHelpers)

find_package(Boost 1.42 REQUIRED)
find_package(range-v3 REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)

set(PYLENE_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")


add_library(Pylene INTERFACE)
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
target_include_directories(Pylene INTERFACE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)


target_include_directories(Pylene SYSTEM INTERFACE ${Boost_INCLUDE_DIR})
target_include_directories(Pylene INTERFACE ${FreeImage_INCLUDE_DIRS})
set_property(TARGET Pylene APPEND
  PROPERTY INTERFACE_LINK_DIRECTORIES ${FreeImage_LIBRARY_DIR} ${Boost_LIBRARY_DIRS})
target_link_libraries(Pylene INTERFACE range-v3)


# Set sources
file(GLOB_RECURSE sources "include/mln/*.hpp")
target_sources(Pylene INTERFACE $<BUILD_INTERFACE:${sources}>)


# Compiler configurations
target_compile_features(Pylene INTERFACE cxx_std_17)


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

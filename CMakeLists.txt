cmake_minimum_required(VERSION 3.8.2)
project(PYLENE)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "${CMAKE_CURRENT_BINARY_DIR}")

find_package(Boost 1.42 COMPONENTS program_options REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)
find_package(Qt4)
find_package(range-v3 REQUIRED)

# CONFIGURATION OPTIONS
set(MLN_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")
option(PYLENE_ENABLE_BENCHMARKS "Require Google Benchmark library. Set to YES to enable the compilation of benchmarks." YES)
option(PYLENE_ENABLE_TESTS      "Require Google Test library. Set to YES to enable the compilation of tests." YES)



# Proper project creation
set(MLN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}")
if(MSVC OR APPLE)
  #dummy static project not linked anywere needed so that a vcxproj is generated with the sources in the solution
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  file(GLOB_RECURSE MLN_SOURCES  "${MLN_INCLUDE_DIR}/*.hpp")
  add_library(Mln_srcs EXCLUDE_FROM_ALL STATIC ${MLN_SOURCES})
  set_target_properties(Mln_srcs PROPERTIES LINKER_LANGUAGE CXX)
  target_include_directories(Mln_srcs PUBLIC ${MLN_INCLUDE_DIR})
  source_group(TREE ${MLN_INCLUDE_DIR} FILES ${MLN_SOURCES})
endif()

add_library(Mln INTERFACE)
target_include_directories(Mln INTERFACE ${MLN_INCLUDE_DIR})
target_link_libraries(Mln INTERFACE range-v3::range-v3)


include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${FreeImage_INCLUDE_DIRS})
link_directories(${FreeImage_LIBRARY_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (TBB_FOUND AND MLN_USE_TBB)
  include_directories(${TBB_INCLUDE_DIRS})
  link_directories(${TBB_LIBRARY_DIRS})
  link_libraries(${TBB_LIBRARIES})
else()
  set(MLN_USE_TBB OFF)
  add_definitions(-DMLN_NO_TBB)
endif()

if (UNIX AND NOT APPLE)
   link_libraries(rt)
endif()

# Compiler configurations
target_compile_features(Mln INTERFACE cxx_std_17)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  add_compile_options("-Wextra" "-Wall" "-Werror" "-Wno-deprecated-declarations")
endif()


#### MSVC Compiler Configuration
if (MSVC)
  add_compile_options("/wd4458" # masque le membre de classe
                      "/wd4456" # masque la déclaration locale précédente
                      "/wd5030" # l'attribut 'noinline' n'est pas reconnu
                      "/wd4457" # la déclaration de '?' masque le paramètre de fonction
  )
  add_compile_options("/WX" "/W4" "/wd4244" "/wd4127" "/permissive-")
  add_compile_definitions(_SCL_SECURE_NO_WARNINGS)
  add_compile_definitions(_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
endif(MSVC)


# default buidling type
if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()


# Testing
ENABLE_TESTING()

#### Configuration of the Compiler ###
# set BOOST to use variadic macros
add_definitions(-D BOOST_PP_VARIADICS=1)



# Subdirectories
if (PYLENE_ENABLE_BENCHMARKS)
  add_subdirectory(bench EXCLUDE_FROM_ALL)
endif()
if (PYLENE_ENABLE_TESTS)
  add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()

add_subdirectory(doc EXCLUDE_FROM_ALL)
add_subdirectory(apps EXCLUDE_FROM_ALL)
add_subdirectory(cli EXCLUDE_FROM_ALL)

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

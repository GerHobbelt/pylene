cmake_minimum_required(VERSION 3.8.2)
project(PYLENE)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
endif ()

find_package(Boost 1.58 COMPONENTS program_options REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)
find_package(Qt4)
find_package(range-v3 REQUIRED)


# CONFIGURE COMPILER LAUNCHERS

option(USE_CCACHE "Use ccache as compiler launcher" ON)
option(USE_DISTCC "Use ccache as compiler launcher" OFF)

if (USE_CCACHE AND USE_DISTCC)
  message(FATAL_ERROR "You can't use both distcc and ccache at the same time. Please choose either one.")
endif (USE_CCACHE AND USE_DISTCC)

# ccache
if (USE_CCACHE)
  find_program(CCACHE_FOUND ccache)
  if (CCACHE_FOUND)
    message(STATUS "ccache found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  endif (CCACHE_FOUND)
endif (USE_CCACHE)

# distcc
if (USE_DISTCC)
  find_program(DISTCC_FOUND distcc)
  if (DISTCC_FOUND)
    message(STATUS "distcc found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE distcc)
    # distcc doesn't offload the linker
    # set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK distcc)
  endif (DISTCC_FOUND)
endif (USE_DISTCC)


# CONFIGURATION OPTIONS

option(PYLENE_BUILD_BENCHMARKS "Require Google Benchmark library. Set to YES to enable the compilation of benchmarks." YES)
option(PYLENE_BUILD_LIBS_ONLY "ON to build only the library (packaging)" OFF)


# Compiler configurations
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  add_compile_options("-Wextra" "-Wall" "-Werror" "-Wno-deprecated-declarations")
endif ()


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
endif (MSVC)


# default buidling type
if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif ()


#### Configuration of the Compiler ###

# Subdirectories
add_subdirectory(pylene)

if (NOT PYLENE_BUILD_LIBS_ONLY)
  add_subdirectory(fixtures)

  if (BUILD_TESTING)
    include(CTest)
    add_custom_target(build-tests)

    add_subdirectory(tests EXCLUDE_FROM_ALL)

    if (PYLENE_BUILD_BENCHMARKS)
      add_subdirectory(bench EXCLUDE_FROM_ALL)
    endif ()
  endif ()

  add_subdirectory(doc EXCLUDE_FROM_ALL)
  add_subdirectory(apps EXCLUDE_FROM_ALL)
  add_subdirectory(cli EXCLUDE_FROM_ALL)
endif ()


set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

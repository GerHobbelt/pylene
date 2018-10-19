cmake_minimum_required(VERSION 3.8.2)
project(PYLENE)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
endif()

find_package(Boost 1.42 COMPONENTS program_options REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)
find_package(Qt4)
find_package(range-v3 REQUIRED)

# CONFIGURATION OPTIONS

option(PYLENE_BUILD_BENCHMARKS "Require Google Benchmark library. Set to YES to enable the compilation of benchmarks." YES)
option(PYLENE_BUILD_LIBS_ONLY "ON to build only the library (packaging)" OFF)


# Compiler configurations
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



#### Configuration of the Compiler ###

# Subdirectories
add_subdirectory(pylene)

if (NOT PYLENE_BUILD_LIBS_ONLY)
  if (PYLENE_BUILD_BENCHMARKS)
    add_subdirectory(bench EXCLUDE_FROM_ALL)
  endif()


  include(CTest)
  if (BUILD_TESTING)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
  endif()


  add_subdirectory(doc EXCLUDE_FROM_ALL)
  add_subdirectory(apps EXCLUDE_FROM_ALL)
  add_subdirectory(cli EXCLUDE_FROM_ALL)
endif()


set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

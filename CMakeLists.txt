cmake_minimum_required(VERSION 3.8.2)
project(Pylene)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
# Add CodeCoverage utility to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake")
endif ()

find_package(Boost 1.58 COMPONENTS program_options REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)
find_package(Qt4)


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
option(PYLENE_BUILD_PYBIND "ON to build the python bindings with pybind11" ON)
option(PYLENE_BUILD_TESTING "ON to build the test suite" ON)

# Compiler configurations
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  add_compile_options(
    "-Wextra"
    "-Wall"
    "-Werror"
    "-Wno-deprecated-declarations"
    )
endif()
#### GCC Compiler Configuration
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    "-Wno-error=maybe-uninitialized" # maybe unitialized may have false alarms
    )
endif()


#### MSVC Compiler Configuration
if (MSVC)
  #string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_compile_options("/wd4458" # masque le membre de classe
                      "/wd4456" # masque la déclaration locale précédente
                      "/wd5030" # l'attribut 'noinline' n'est pas reconnu
                      "/wd4457" # la déclaration de '?' masque le paramètre de fonction
                      "/wd4996" # '...': was declared deprecated
                      )
  add_compile_options("/WX" "/W3" "/wd4244" "/wd4127" "/permissive-")
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
  add_custom_target(build-fixtures)
  add_subdirectory(fixtures)

  if (PYLENE_BUILD_TESTING)
    include(CTest)
    add_custom_target(build-tests)

    if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
      option(PYLENE_CODE_COVERAGE "ON to enable code coverage" OFF)
      if (PYLENE_CODE_COVERAGE)
        include(CodeCoverage)
        APPEND_COVERAGE_COMPILER_FLAGS()

        SETUP_TARGET_FOR_COVERAGE_LCOV(
          NAME run-tests_coverage-lcov
          EXECUTABLE ctest -L UnitTests --schedule-random --output-on-failure -j ${PROCESSOR_COUNT}
          DEPENDENCIES build-tests
        )

        SETUP_TARGET_FOR_COVERAGE_GCOVR_XML(
          NAME run-tests_coverage-gcovr-xml
          EXECUTABLE ctest -L UnitTests --schedule-random --output-on-failure -j ${PROCESSOR_COUNT}
          DEPENDENCIES build-tests
        )

        SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
          NAME run-tests_coverage-gcovr-html
          EXECUTABLE ctest -L UnitTests --schedule-random --output-on-failure -j ${PROCESSOR_COUNT}
          DEPENDENCIES build-tests
        )

      endif ()
    endif ()

    add_subdirectory(tests EXCLUDE_FROM_ALL)

    if (PYLENE_BUILD_BENCHMARKS)
      add_custom_target(build-bench)
      add_subdirectory(bench EXCLUDE_FROM_ALL)
    endif ()
  endif ()

  add_custom_target(build-doc)
  add_subdirectory(doc EXCLUDE_FROM_ALL)
  add_custom_target(build-apps)
  add_subdirectory(apps EXCLUDE_FROM_ALL)
endif ()

if (PYLENE_BUILD_PYBIND)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  add_custom_target(build-pybind)
  add_subdirectory(pybind)
endif()

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

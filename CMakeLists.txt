cmake_minimum_required(VERSION 3.14)
project(Anonymous)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "${CMAKE_BINARY_DIR}")

find_package(FreeImage REQUIRED)

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
option(ANONYMOUS_BUILD_LIBS_ONLY "ON to build only the library (packaging)" OFF)
option(ANONYMOUS_BUILD_TESTING "ON to build the test suite" OFF)

# Compiler configurations
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} compiler minimum version required is 10 (Got ${CMAKE_CXX_COMPILER_VERSION})")
  endif()
  add_compile_options(
    "-Wextra"
    "-Wall"
    # "-Werror"
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
  add_compile_options("/WX" "/W3" "/wd4244" "/wd4127")
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

macro(set_alternate_linker linker)
  find_program(LINKER_EXECUTABLE ld.${USE_ALTERNATE_LINKER} ${USE_ALTERNATE_LINKER})
  if(LINKER_EXECUTABLE)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 12.0.0)
      add_link_options("-ld-path=${USE_ALTERNATE_LINKER}")
    else()
      add_link_options("-fuse-ld=${USE_ALTERNATE_LINKER}")
    endif()
  else()
    set(USE_ALTERNATE_LINKER "" CACHE STRING "Use alternate linker" FORCE)
  endif()
endmacro()

set(USE_ALTERNATE_LINKER "" CACHE STRING "Use alternate linker. Leave empty for system default; alternatives are 'gold', 'lld', 'bfd', 'mold'")
if(NOT "${USE_ALTERNATE_LINKER}" STREQUAL "")
  set_alternate_linker(${USE_ALTERNATE_LINKER})
endif()

# Subdirectories
add_library(Anonymous INTERFACE)
add_library(Anonymous::Anonymous ALIAS Anonymous)
add_subdirectory(anonymous)

if (NOT ANONYMOUS_BUILD_LIBS_ONLY)
  add_custom_target(build-fixtures)
  add_subdirectory(fixtures)

  add_custom_target(build-apps)
  add_subdirectory(apps EXCLUDE_FROM_ALL)
endif ()


####################################################
####### Installation & CMAKE module files ##########
####################################################

target_link_libraries(Anonymous INTERFACE Anonymous::Core)

include(GNUInstallDirs)

set(AnonymousTargets Anonymous Anonymous-core Anonymous-bp Anonymous-io-freeimage)
set(OptionalAnonymousTargets Anonymous-io-fits Anonymous-numpy Anonymous-scribo)

foreach(tgt ${OptionalAnonymousTargets})
  if (TARGET ${tgt})
    list(APPEND AnonymousTargets ${tgt})
  endif()
endforeach()

install(TARGETS ${AnonymousTargets}
        EXPORT AnonymousTargets)

write_basic_package_version_file(
    anonymous/AnonymousConfigVersion.cmake
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
    )

configure_file(
    anonymous/AnonymousConfig.cmake.in
    anonymous/AnonymousConfig.cmake
    @ONLY
    )

install(EXPORT AnonymousTargets
        FILE AnonymousTargets.cmake
        NAMESPACE Anonymous::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/anonymous
        )

install(DIRECTORY anonymous/include/ano
        TYPE INCLUDE)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/anonymous/AnonymousConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/anonymous/AnonymousConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/anonymous)

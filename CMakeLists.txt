cmake_minimum_required(VERSION 2.8)
project(MILENA)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(Boost 1.42 COMPONENTS program_options unit_test_framework REQUIRED)
find_package(FreeImage REQUIRED)
find_package(TBB)
find_package(Qt4)

if(TBB_FOUND)
        MESSAGE (STATUS "TBB found in ${TBB_INSTALL_DIR} + ${TBB_LIBRARY_DIRS}")
endif()

# CONFIGURATION OPTIONS
set(MLN_USE_TBB YES CACHE BOOL "Set to NO to disable use of TBB and parallelization")



# add source directory to find mln path
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${FreeImage_INCLUDE_DIRS})
link_directories(${FreeImage_LIBRARY_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (TBB_FOUND AND MLN_USE_TBB)
  include_directories(${TBB_INCLUDE_DIRS})
  link_directories(${TBB_LIBRARY_DIRS})
  link_libraries(${TBB_LIBRARIES})
else()
  add_definitions(-DMLN_NO_TBB)
endif()

if (UNIX AND NOT APPLE)
   link_libraries(rt)
endif()


if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wextra -Wall -std=c++1z -Werror")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g -ggdb")
elseif (MSVC)  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX " )
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

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
add_subdirectory(tests EXCLUDE_FROM_ALL)
add_subdirectory(bench EXCLUDE_FROM_ALL)
add_subdirectory(apps EXCLUDE_FROM_ALL)
add_subdirectory(cli EXCLUDE_FROM_ALL)

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

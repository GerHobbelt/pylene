cmake_minimum_required(VERSION 2.8)
project(MILENA)

include(FindBoost)

find_package(Boost 1.42 COMPONENTS unit_test_framework REQUIRED)

if(Boost_FOUND)
 message(STATUS, "Boost trouvé ${Boost_INCLUDE_DIRS}")
 include_directories(${Boost_INCLUDE_DIRS})
else()
 message(FATAL_ERROR, "Boost pas trouvé")
endif()



# add source directory to find mln path
include_directories("${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "-W -Wextra -Wall -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "-W -Wextra -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-W -Wextra -Wall -O3 -DNDEBUG -march=native")


# 
ENABLE_TESTING()
SUBDIRS(tests)

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)


add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

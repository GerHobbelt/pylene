cmake_minimum_required(VERSION 2.8)
project(MILENA)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(FindBoost)



find_package(Boost 1.42 COMPONENTS program_options unit_test_framework REQUIRED)
find_package(TBB)

if(TBB_FOUND)
        MESSAGE (STATUS "TBB found in ${TBB_INSTALL_DIR}")
else()
        MESSAGE (FATAL "TBB not found.")
endif()

if(Boost_FOUND)
 message(STATUS, "Boost trouvé ${Boost_INCLUDE_DIRS}")
 include_directories(${Boost_INCLUDE_DIRS})
else()
 message(FATAL_ERROR, "Boost pas trouvé")
endif()



# add source directory to find mln path
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(${TBB_INCLUDE_DIRS})
link_directories(${TBB_LIBRARY_DIRS})
link_libraries(${TBB_LIBRARIES} rt)

set(CMAKE_CXX_FLAGS "-W -Wextra -Wall -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "-W -Wextra -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-W -Wextra -Wall -O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-W -Wextra -Wall -O3 -DNDEBUG -g -march=native")


# 
ENABLE_TESTING()
SUBDIRS(tests)

set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/;/\\\\.git/;.*~;#.*#")
INCLUDE(CPack)


add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

#SUBDIRS(labeling)
#find_package(benchmark)



add_definitions(-DMLN_IMG_PATH="${CMAKE_SOURCE_DIR}/img/")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  link_libraries(-lpthread)
endif()



if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message("compiler id is " ${CMAKE_CXX_COMPILER_ID})
  set(STANDALONE_COMPILE_FLAGS "-Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only -gcolumn-info ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(STANDALONE_COMPILE_FLAGS "-fopt-info")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
else ()
   set(STANDALONE_COMPILE_FLAGS " ")
endif ()

set(src_standalone
  BMErosion.cpp
  BMMorphers.cpp
  BMNeighborhood.cpp
  )

set_source_files_properties(${src_standalone} PROPERTIES COMPILE_FLAGS ${STANDALONE_COMPILE_FLAGS})

#link_libraries(benchmark::benchmark)
link_libraries(-lbenchmark)

add_executable(BMErosion        BMErosion.cpp BMErosion_main.cpp)
add_executable(BMNeighborhood   BMNeighborhood.cpp BMNeighborhood_main.cpp)
add_executable(BMRotation       BMRotation.cpp)
add_executable(BMMorphers       BMMorphers.cpp   BMMorphers_main.cpp)
target_link_libraries(BMRotation ${FreeImage_LIBRARIES})
target_link_libraries(BMMorphers ${FreeImage_LIBRARIES})
target_link_libraries(BMErosion ${FreeImage_LIBRARIES})

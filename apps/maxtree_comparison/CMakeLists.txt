add_executable(maxtree_serial_najman	bench_serial_najman.cpp)
add_executable(maxtree_serial_ufind     bench_serial_ufind.cpp)
add_executable(maxtree_serial_berger    bench_serial_ufind.cpp)
add_executable(maxtree_serial_ufind_wlc bench_serial_ufind.cpp)
add_executable(maxtree_serial_pqueue	bench_serial_pqueue.cpp)
add_executable(maxtree_serial_wilkinson bench_serial_pqueue.cpp)
add_executable(maxtree_serial_nister    bench_serial_pqueue.cpp)
add_executable(maxtree_serial_ufindrank bench_serial_ufindrank.cpp)
add_executable(maxtree_serial_hqueue	bench_serial_hqueue.cpp)

set_target_properties(maxtree_serial_berger	PROPERTIES COMPILE_DEFINITIONS NO_LEVEL_COMPRESSION)
set_target_properties(maxtree_serial_ufind_wlc	PROPERTIES COMPILE_DEFINITIONS LEVEL_COMPRESSION)
set_target_properties(maxtree_serial_wilkinson	PROPERTIES COMPILE_DEFINITIONS "PQUEUE_FAST_SWITCH_HQUEUE_NBITS=1")
set_target_properties(maxtree_serial_nister	PROPERTIES COMPILE_DEFINITIONS "PQUEUE_FAST_SWITCH_HQUEUE_NBITS=100")

target_link_libraries(maxtree_serial_najman	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_ufind	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_berger	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_ufind_wlc	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_pqueue	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_wilkinson	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_nister	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_ufindrank	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_serial_hqueue	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})



add_executable(maxtree_parallel_ufind		bench_parallel_ufind.cpp)
add_executable(maxtree_parallel_ufind_line	bench_parallel_ufind_line.cpp)
add_executable(maxtree_parallel_ufindrank	bench_parallel_ufindrank.cpp)
add_executable(maxtree_parallel_hqueue		bench_parallel_hqueue.cpp)
add_executable(maxtree_parallel_pqueue		bench_parallel_pqueue.cpp)

target_link_libraries(maxtree_parallel_ufind		${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_parallel_ufind_line	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_parallel_ufindrank	${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_parallel_hqueue		${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(maxtree_parallel_pqueue		${FreeImage_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(maxtree-cli		maxtree-cli.cpp)
target_link_libraries(maxtree-cli	${FreeImage_LIBRARIES})

add_executable(filter-cli		filter-cli.cpp)
target_link_libraries(filter-cli	${FreeImage_LIBRARIES})

# add link flag to test framework
link_libraries(${Boost_LIBRARIES})
add_definitions(-DBOOST_TEST_DYN_LINK)

SUBDIRS(core io)


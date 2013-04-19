# add link flag to test framework
link_libraries(${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_definitions(-DBOOST_TEST_DYN_LINK)

SUBDIRS(core draw io colors morpho accu labeling graph graphcut)


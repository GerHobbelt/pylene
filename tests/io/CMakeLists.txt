find_package(cfitsio)

add_core_test(UTIo_freeimage freeimage.cpp)
add_core_test(UTIo_imprint imprint.cpp)
#if (cfitsio_FOUND)
#  add_core_test(UTIo_cfitsio cfitsio.cpp)
#  target_link_libraries(UTIo_cfitsio PUBLIC Pylene::IO-fits)
#endif(cfitsio_FOUND)

add_library(labelize_serial labelize_serial.cpp)
target_link_libraries(labelize_serial PRIVATE Pylene::Pylene Pylene::IO-freeimage)
target_compile_features(labelize_serial PRIVATE cxx_std_20)

add_library(labelize_view labelize_view.cpp)
target_link_libraries(labelize_view PRIVATE Pylene::Pylene Pylene::IO-freeimage)
target_compile_features(labelize_view PRIVATE cxx_std_20)

add_dependencies(build-apps labelize_view labelize_serial)

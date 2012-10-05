add_executable(bench_iterator ../bench_iterator.cpp)
add_executable(bench_zip_iterator ../bench_zip_iterator.cpp)

add_executable(image2d image/image2d.cpp)



add_executable(array std/array.cpp)
add_executable(zip_image image/zip_image.cpp)
add_executable(image_ops image/image_ops.cpp)


add_executable(copy algorithm/copy.cpp)
add_executable(fill algorithm/fill.cpp)
add_executable(paste algorithm/paste.cpp)
add_executable(equal algorithm/equal.cpp)


add_test(image2d image2d)
add_test(zip_image zip_image)
add_test(image_ops image_ops)
add_test(copy copy)
add_test(fill fill)
add_test(paste paste)
add_test(equal equal)

add_executable(bench_iterator ../bench_iterator.cpp)
add_executable(bench_zip_iterator ../bench_zip_iterator.cpp)


add_executable(array std/array.cpp)

# test Domain
add_executable(box domain/box.cpp)

add_test(box box)

# test Range
add_executable(range_filter range/filter.cpp)

add_test(range_filter range_filter)

# test Images
add_executable(image2d   image/image2d.cpp)
add_executable(extended_by_value_image image/morphers/extended_by_value.cpp)
add_executable(filtered_image image/morphers/filtered_image.cpp)
add_executable(zip_image image/zip_image.cpp)
add_executable(sub_image image/sub_image.cpp)
add_executable(image_ops image/image_ops.cpp)
add_executable(image_routines image/image_routines.cpp)


add_test(image2d   image2d)
add_test(extended_by_value_image extended_by_value_image)
add_test(filtered_image filtered_image)
add_test(zip_image zip_image)
add_test(image_ops image_ops)
add_test(image_routines image_routines)
add_test(sub_image sub_image)

# test Extension
add_executable(extension_fill extension/fill.cpp)

add_test(extension_fill extension_fill)

# test Value
add_executable(value_int value/int.cpp)

add_test(value_int value_int)


# test Algorithms

add_executable(copy algorithm/copy.cpp)
add_executable(fill algorithm/fill.cpp)
add_executable(paste algorithm/paste.cpp)
add_executable(equal algorithm/equal.cpp)
add_executable(accumulate algorithm/accumulate.cpp)
add_executable(sort_indexes algorithm/sort_indexes.cpp)


add_test(copy copy)
add_test(accumulate accumulate)
add_test(fill fill)
add_test(paste paste)
add_test(equal equal)
add_test(sort_indexes sort_indexes)


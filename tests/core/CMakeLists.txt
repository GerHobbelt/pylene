add_executable(bench_iterator ../bench_iterator.cpp)
add_executable(bench_zip_iterator ../bench_zip_iterator.cpp)

set(test_prefix "UTCore_")
add_core_test(${test_prefix}win2d win2d.cpp)

# test Domain
add_core_test(${test_prefix}box domain/box.cpp)

# test loop
#add_core_test(${test_prefix}forloop forloop.cpp)

# test Range
#add_core_test(${test_prefix}range range/range.cpp)
#add_core_test(${test_prefix}range_filter range/filter.cpp)
#add_core_test(${test_prefix}range_zip range/zip.cpp)


# test Images
#add_core_test(${test_prefix}image2d   image/image2d.cpp)
#add_core_test(${test_prefix}image2d_iterators   image/image2d_iterators.cpp)
#add_core_test(${test_prefix}extended_by_value_image image/morphers/extended_by_value.cpp)
#add_core_test(${test_prefix}transformed_image image/morphers/transformed_image.cpp)
#add_core_test(${test_prefix}filtered_image image/morphers/filtered_image.cpp)
#add_core_test(${test_prefix}zip_image image/zip_image.cpp)
#add_core_test(${test_prefix}sub_image image/sub_image.cpp)
#add_core_test(${test_prefix}image_ops image/image_ops.cpp)
#add_core_test(${test_prefix}image_routines image/image_routines.cpp)

# test Neighborhood
#add_core_test(${test_prefix}sliding_pixter   neighborhood/sliding_pixter.cpp)
#add_core_test(${test_prefix}sliding_wpiter   neighborhood/sliding_wpiter.cpp)
#add_core_test(${test_prefix}sliding_wpixter  neighborhood/sliding_wpixter.cpp)
#add_core_test(${test_prefix}wneighborhood    neighborhood/wneighborhood.cpp)

# test Extension
#add_core_test(${test_prefix}extension_fill     extension/fill.cpp)

# test Value
#add_core_test(${test_prefix}value_int          value/int.cpp)
#add_core_test(${test_prefix}vec_math_ops       vec/vec_math_ops.cpp)

# test Algorithms
#add_core_test(${test_prefix}copy               algorithm/copy.cpp)
#add_core_test(${test_prefix}fill               algorithm/fill.cpp)
#add_core_test(${test_prefix}paste              algorithm/paste.cpp)
#add_core_test(${test_prefix}equal              algorithm/equal.cpp)
#add_core_test(${test_prefix}accumulate         algorithm/accumulate.cpp)
#add_core_test(${test_prefix}sort_indexes       algorithm/sort_indexes.cpp)

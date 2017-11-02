add_executable(bench_iterator ../bench_iterator.cpp)
add_executable(bench_zip_iterator ../bench_zip_iterator.cpp)


set(test_prefix "UTCore_")
add_executable(${test_prefix}win2d win2d.cpp)

# test Domain
add_executable(${test_prefix}box domain/box.cpp)

# test loop
add_executable(${test_prefix}forloop forloop.cpp)

# test Range
add_executable(${test_prefix}range range/range.cpp)
add_executable(${test_prefix}range_filter range/filter.cpp)
add_executable(${test_prefix}range_zip range/zip.cpp)


# test Images
add_executable(${test_prefix}image2d   image/image2d.cpp)
add_executable(${test_prefix}image2d_iterators   image/image2d_iterators.cpp)
add_executable(${test_prefix}extended_by_value_image image/morphers/extended_by_value.cpp)
add_executable(${test_prefix}transformed_image image/morphers/transformed_image.cpp)
add_executable(${test_prefix}filtered_image image/morphers/filtered_image.cpp)
add_executable(${test_prefix}zip_image image/zip_image.cpp)
add_executable(${test_prefix}sub_image image/sub_image.cpp)
add_executable(${test_prefix}image_ops image/image_ops.cpp)
add_executable(${test_prefix}image_routines image/image_routines.cpp)

# test Neighborhood
add_executable(${test_prefix}sliding_pixter   neighborhood/sliding_pixter.cpp)
add_executable(${test_prefix}sliding_wpiter   neighborhood/sliding_wpiter.cpp)
add_executable(${test_prefix}sliding_wpixter  neighborhood/sliding_wpixter.cpp)
add_executable(${test_prefix}wneighborhood    neighborhood/wneighborhood.cpp)

# test Extension
add_executable(${test_prefix}extension_fill     extension/fill.cpp)

# test Value
add_executable(${test_prefix}value_int          value/int.cpp)
add_executable(${test_prefix}vec_math_ops       vec/vec_math_ops.cpp)

# test Algorithms

add_executable(${test_prefix}copy               algorithm/copy.cpp)
add_executable(${test_prefix}fill               algorithm/fill.cpp)
add_executable(${test_prefix}paste              algorithm/paste.cpp)
add_executable(${test_prefix}equal              algorithm/equal.cpp)
add_executable(${test_prefix}accumulate         algorithm/accumulate.cpp)
add_executable(${test_prefix}sort_indexes       algorithm/sort_indexes.cpp)



add_core_test(${test_prefix}win2d                       ${test_prefix}win2d)
add_core_test(${test_prefix}box                         ${test_prefix}box)
add_core_test(${test_prefix}range                       ${test_prefix}range)
add_core_test(${test_prefix}range_filter                ${test_prefix}range_filter)
add_core_test(${test_prefix}range_zip                   ${test_prefix}range_zip)
add_core_test(${test_prefix}image2d                     ${test_prefix}image2d)
add_core_test(${test_prefix}extended_by_value_image     ${test_prefix}extended_by_value_image)
add_core_test(${test_prefix}filtered_image              ${test_prefix}filtered_image)
add_core_test(${test_prefix}transformed_image           ${test_prefix}transformed_image)
add_core_test(${test_prefix}zip_image                   ${test_prefix}zip_image)
add_core_test(${test_prefix}image_ops                   ${test_prefix}image_ops)
add_core_test(${test_prefix}image_routines              ${test_prefix}image_routines)
add_core_test(${test_prefix}sub_image                   ${test_prefix}sub_image)
add_core_test(${test_prefix}extension_fill              ${test_prefix}extension_fill)
add_core_test(${test_prefix}value_int                   ${test_prefix}value_int)
add_core_test(${test_prefix}vec_math_ops                ${test_prefix}vec_math_ops)
add_core_test(${test_prefix}copy                        ${test_prefix}copy)
add_core_test(${test_prefix}accumulate                  ${test_prefix}accumulate)
add_core_test(${test_prefix}fill                        ${test_prefix}fill)
add_core_test(${test_prefix}paste                       ${test_prefix}paste)
add_core_test(${test_prefix}equal                       ${test_prefix}equal)
add_core_test(${test_prefix}sort_indexes                ${test_prefix}sort_indexes)


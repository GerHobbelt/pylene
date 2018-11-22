set(test_prefix "UTCore_")

# test Domain
add_core_test(${test_prefix}box domain/box.cpp)

# test loop
add_core_test(${test_prefix}forloop forloop.cpp)

# test Range
add_core_test(${test_prefix}range               range/range.cpp)
add_core_test(${test_prefix}range_foreach       range/foreach.cpp)
add_core_test(${test_prefix}range_multi_indices       range/multi_indices.cpp)
add_core_test(${test_prefix}range_multi_span    range/multi_span.cpp)
add_core_test(${test_prefix}range_transform     range/transform.cpp)
add_core_test(${test_prefix}range_filter        range/filter.cpp)
add_core_test(${test_prefix}range_reverse       range/reverse.cpp)
add_core_test(${test_prefix}range_zip           range/zip.cpp)

# test Views
add_core_test(${test_prefix}view_transform   image/view/transform.cpp)
add_core_test(${test_prefix}view_zip         image/view/zip.cpp)


# test Images
add_core_test(${test_prefix}image2d             image/image2d.cpp)
add_core_test(${test_prefix}image2d_iterators   image/image2d_iterators.cpp)
add_core_test(${test_prefix}extended_by_value_image image/morphers/extended_by_value.cpp)
add_core_test(${test_prefix}transformed_image   image/morphers/transformed_image.cpp)
add_core_test(${test_prefix}filtered_image      image/morphers/filtered_image.cpp)
add_core_test(${test_prefix}zip_image           image/zip_image.cpp)
add_core_test(${test_prefix}sub_image           image/sub_image.cpp)
add_core_test(${test_prefix}image_ops           image/image_ops.cpp)
add_core_test(${test_prefix}image_routines      image/image_routines.cpp)

# test Neighborhood
add_core_test(${test_prefix}c4               neighborhood/c4.cpp)
add_core_test(${test_prefix}c8               neighborhood/c8.cpp)
add_core_test(${test_prefix}sliding_pixter   neighborhood/sliding_pixter.cpp)
add_core_test(${test_prefix}sliding_wpiter   neighborhood/sliding_wpiter.cpp)
add_core_test(${test_prefix}sliding_wpixter  neighborhood/sliding_wpixter.cpp)
add_core_test(${test_prefix}wneighborhood    neighborhood/wneighborhood.cpp)

# test Extension
add_core_test(${test_prefix}extension_fill     extension/fill.cpp)

# test Value
add_core_test(${test_prefix}value_int          value/int.cpp)

# test Vec
add_core_test(${test_prefix}vec_math_ops       vec/vec_math_ops.cpp)

# test Algorithms
add_core_test(${test_prefix}copy               algorithm/copy.cpp)
add_core_test(${test_prefix}fill               algorithm/fill.cpp)
add_core_test(${test_prefix}paste              algorithm/paste.cpp)
add_core_test(${test_prefix}equal              algorithm/equal.cpp)
add_core_test(${test_prefix}accumulate         algorithm/accumulate.cpp)
add_core_test(${test_prefix}sort_indexes       algorithm/sort_indexes.cpp)

# test SE
add_core_test(${test_prefix}disc               se/disc.cpp)
add_core_test(${test_prefix}rect2d             se/rect2d.cpp)

# test Concepts
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.2)
  add_core_test(${test_prefix}concept_stl      concept/fundamentals.cpp)
  add_core_test(${test_prefix}concept_values   concept/values.cpp)
  add_core_test(${test_prefix}concept_points   concept/points.cpp)
  add_core_test(${test_prefix}concept_pixels   concept/pixels.cpp)
  add_core_test(${test_prefix}concept_domains  concept/domains.cpp)
endif()

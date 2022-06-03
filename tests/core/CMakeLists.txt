set(test_prefix "UTCore_")

# test Core components
add_core_test(${test_prefix}point point.cpp)
add_core_test(${test_prefix}box box.cpp)

# test Range
add_core_test(${test_prefix}range_foreach       range/foreach.cpp)
add_core_test(${test_prefix}range_transform     range/transform.cpp)
add_core_test(${test_prefix}range_transform_if  range/transform_if.cpp)
add_core_test(${test_prefix}range_filter        range/filter.cpp)
add_core_test(${test_prefix}range_reverse       range/reverse.cpp)
add_core_test(${test_prefix}range_zip           range/zip.cpp)
add_core_test(${test_prefix}range_mdspan        range/mdspan.cpp)
add_core_test(${test_prefix}range_mdindex       range/mdindex.cpp)

# test padding functionalities
add_core_test(${test_prefix}padding             extension/padding.cpp)



# test Views
add_core_test(${test_prefix}view_adaptor            image/view/adaptor.cpp)
add_core_test(${test_prefix}view_cast               image/view/cast.cpp)
add_core_test(${test_prefix}view_channel            image/view/channel.cpp)
add_core_test(${test_prefix}view_clip               image/view/clip.cpp)
add_core_test(${test_prefix}view_value_extended     image/view/value_extended.cpp)
add_core_test(${test_prefix}view_mirror_extended    image/view/mirror_extended.cpp)
add_core_test(${test_prefix}view_image_extended     image/view/image_extended.cpp)
add_core_test(${test_prefix}view_none_extended      image/view/none_extended.cpp)
add_core_test(${test_prefix}view_periodize_extended image/view/periodize_extended.cpp)
add_core_test(${test_prefix}view_clamp_extended     image/view/clamp_extended.cpp)
add_core_test(${test_prefix}view_extended           image/view/extended.cpp)
add_core_test(${test_prefix}view_filter             image/view/filter.cpp)
add_core_test(${test_prefix}view_mask               image/view/mask.cpp)
add_core_test(${test_prefix}view_maths_comps        image/view/maths_components.cpp)
add_core_test(${test_prefix}view_maths_reds         image/view/maths_reductions.cpp)
add_core_test(${test_prefix}view_maths_norms        image/view/maths_norms.cpp)
add_core_test(${test_prefix}view_maths_dists        image/view/maths_distances.cpp)
add_core_test(${test_prefix}view_ops                image/view/operators.cpp)
add_core_test(${test_prefix}view_rgb                image/view/rgb.cpp)
add_core_test(${test_prefix}view_transform          image/view/transform.cpp)
add_core_test(${test_prefix}view_zip                image/view/zip.cpp)


# test Images
add_core_test(${test_prefix}image_ndbuffer_image    image/ndbuffer_image.cpp)
add_core_test(${test_prefix}image_ndimage           image/ndimage.cpp)




# test Neighborhood
add_core_test(${test_prefix}c4               neighborhood/c4.cpp)
add_core_test(${test_prefix}c6               neighborhood/c6.cpp)
add_core_test(${test_prefix}c8               neighborhood/c8.cpp)
add_core_test(${test_prefix}c26              neighborhood/c26.cpp)
add_core_test(${test_prefix}dyn_nbh_2d       neighborhood/dyn_nbh_2d.cpp)

# test Extension
add_core_test(${test_prefix}extension_clamp     extension/clamp.cpp)
add_core_test(${test_prefix}extension_fill      extension/fill.cpp)
add_core_test(${test_prefix}extension_mirror    extension/mirror.cpp)
add_core_test(${test_prefix}extension_periodize extension/periodize.cpp)


# test Value
add_core_test(${test_prefix}value_int          value/int.cpp)

# test Vec
add_core_test(${test_prefix}vec_math_ops       vec/vec_math_ops.cpp)

# test Algorithms
add_core_test(${test_prefix}all_any_none_of    algorithm/all_any_none_of.cpp)
add_core_test(${test_prefix}copy               algorithm/copy.cpp)
add_core_test(${test_prefix}clone              algorithm/clone.cpp)
add_core_test(${test_prefix}count              algorithm/count.cpp)
add_core_test(${test_prefix}count_if           algorithm/count_if.cpp)
add_core_test(${test_prefix}fill               algorithm/fill.cpp)
add_core_test(${test_prefix}for_each           algorithm/for_each.cpp)
add_core_test(${test_prefix}iota               algorithm/iota.cpp)
add_core_test(${test_prefix}generate           algorithm/generate.cpp)
add_core_test(${test_prefix}paste              algorithm/paste.cpp)
add_core_test(${test_prefix}equal              algorithm/equal.cpp)
add_core_test(${test_prefix}accumulate         algorithm/accumulate.cpp)
add_core_test(${test_prefix}transform          algorithm/transform.cpp)
add_core_test(${test_prefix}sort               algorithm/sort.cpp)
add_core_test(${test_prefix}accumulate_local   algorithm/accumulate_local.cpp)
target_link_libraries(${test_prefix}clone PRIVATE TBB::tbb)
target_link_libraries(${test_prefix}copy PRIVATE TBB::tbb)
target_link_libraries(${test_prefix}fill PRIVATE TBB::tbb)
target_link_libraries(${test_prefix}for_each PRIVATE TBB::tbb)
target_link_libraries(${test_prefix}paste PRIVATE TBB::tbb)
target_link_libraries(${test_prefix}transform PRIVATE TBB::tbb)


# test SE
add_core_test(${test_prefix}periodic_line2d    se/periodic_line2d.cpp)
add_core_test(${test_prefix}disc               se/disc.cpp)
add_core_test(${test_prefix}rect2d             se/rect2d.cpp)
add_core_test(${test_prefix}mask2d             se/mask2d.cpp)


# Others
add_core_test(${test_prefix}traverse2d                   canvas/traverse2d.cpp)

# test Concepts
# Add concepts support for gcc > 7.2 with -fconcepts
if (PYLENE_CONCEPT_TS_ENABLED)
  add_core_test(${test_prefix}concept_values            concept/values.cpp)
  add_core_test(${test_prefix}concept_points            concept/points.cpp)
  add_core_test(${test_prefix}concept_pixels            concept/pixels.cpp)
  add_core_test(${test_prefix}concept_domains           concept/domains.cpp)
  add_core_test(${test_prefix}concept_indexes           concept/indexes.cpp)
  add_core_test(${test_prefix}concept_ranges            concept/ranges.cpp)
  add_core_test(${test_prefix}concept_images            concept/images.cpp)
  add_core_test(${test_prefix}concept_se                concept/se.cpp)
  add_core_test(${test_prefix}concept_neighborhood      concept/neighborhood.cpp)
endif()

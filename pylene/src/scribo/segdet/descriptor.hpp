#pragma once

#include <scribo/segdet.hpp>

#include <Eigen/Dense>
#include <algorithm>

namespace scribo::internal
{
  struct Descriptor
  {
    /// PREPROCESS
    SEGDET_PREPROCESS_ENUM preprocess;
    // BLACK_TOP_HAT
    float dyn;
    int   size_mask;


    /// PROCESS
    SEGDET_PROCESS_TRAVERSAL_MODE_ENUM traversal_mode;
    int                                bucket_size;
    int                                min_length_embryo;
    int                                discontinuity_absolute;
    double                             discontinuity_relative;
    int                                minimum_for_fusion;
    float                              max_slope;

    // MATCHING
    int   nb_values_to_keep;
    int   default_sigma_position;
    int   default_sigma_thickness;
    int   default_sigma_luminosity;
    int   min_nb_values_sigma;
    float sigma_pos_min;
    float sigma_thickness_min;
    float sigma_luminosity_min;

    // EXTRACTION
    int   max_llum;
    int   max_max_llum;
    int   max_thickness;
    float ratio_lum;

    // MODULE
    SEGDET_PROCESS_TRACKING_ENUM filter_type;


    /// POSTPROCESS
    bool  remove_duplicates;
    float threshold_intersection;
    int   min_length;

    Descriptor(const SegDetParams& params, int min_length);
  };

} // namespace scribo::internal
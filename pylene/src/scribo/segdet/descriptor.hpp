#pragma once

#include <scribo/segdet.hpp>

#include <Eigen/Dense>
#include <algorithm>

namespace scribo::internal
{
  struct Descriptor
  {
    /// PREPROCESS
    bool                negate_image;
    e_segdet_preprocess preprocess;
    // BLACK_TOP_HAT
    float dyn;
    int   size_mask;


    /// PROCESS
    e_segdet_process_traversal_mode traversal_mode;
    int                             bucket_size;
    int                             min_length_embryo;
    int                             discontinuity_absolute;
    double                          discontinuity_relative;
    int                             minimum_for_fusion;

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
    e_segdet_process_extraction extraction_type;
    int                         gradient_threshold;
    int                         llumi;
    int                         blumi;
    int                         max_thickness;
    float                       ratio_lum;

    // MODULE
    e_segdet_process_tracking tracking_type;
    float                     double_exponential_alpha;
    float                     simple_moving_average_memory;
    float                     exponential_moving_average_memory;
    float                     one_euro_beta;
    float                     one_euro_mincutoff;
    float                     one_euro_dcutoff;


    /// POSTPROCESS
    bool  remove_duplicates;
    float threshold_intersection;
    int   min_length;

    Descriptor(const SegDetParams& params, int min_length);
  };

} // namespace scribo::internal
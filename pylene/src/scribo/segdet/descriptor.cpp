#include "descriptor.hpp"

namespace scribo::internal
{
  Descriptor::Descriptor(const SegDetParams& params, int min_length)
  {
    negate_image = params.negate_image;
    preprocess   = params.preprocess;
    if (params.preprocess == scribo::SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT)
    {
      dyn       = params.dyn;
      size_mask = params.size_mask;
    }

    traversal_mode         = params.traversal_mode;
    bucket_size            = params.bucket_size;
    min_length_embryo      = min_length / 4 + 1;
    discontinuity_relative = static_cast<double>(params.discontinuity_relative) / 100;
    discontinuity_absolute = params.discontinuity_absolute;
    minimum_for_fusion     = params.minimum_for_fusion;
    max_slope              = std::tan(params.max_slope * 3.14 / 180);

    nb_values_to_keep        = params.nb_values_to_keep;
    default_sigma_position   = params.default_sigma_position;
    default_sigma_thickness  = params.default_sigma_thickness;
    default_sigma_luminosity = params.default_sigma_luminosity;
    min_nb_values_sigma      = params.min_nb_values_sigma;
    sigma_pos_min            = params.sigma_pos_min;
    sigma_thickness_min      = params.sigma_thickness_min;
    sigma_luminosity_min     = params.sigma_luminosity_min;

    extraction_type    = params.extraction_type;
    gradient_threshold = params.gradient_threshold;
    max_llum           = params.max_llum;
    max_max_llum       = params.max_max_llum;
    max_thickness      = params.max_thickness;
    ratio_lum          = params.ratio_lum;

    filter_type                  = params.tracker;
    double_exponential_alpha     = params.double_exponential_alpha;
    simple_moving_average_memory = params.simple_moving_average_memory;


    threshold_intersection = params.threshold_intersection;
    remove_duplicates      = params.remove_duplicates;
    this->min_length       = min_length;
  }
} // namespace scribo::internal
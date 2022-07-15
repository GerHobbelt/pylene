#include "descriptor.hpp"

namespace scribo::internal
{
  Descriptor::Descriptor(const SegDetParams& params, int min_length)
  {
    preprocess = params.preprocess;
    if (params.preprocess == scribo::SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT)
    {
      dyn       = params.dyn;
      size_mask = params.size_mask;
    }

    traversal_mode         = params.traversal_mode;
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

    max_llum      = params.max_llum;
    max_max_llum  = params.max_max_llum;
    max_thickness = params.max_thickness;
    ratio_lum     = params.ratio_lum;

    filter_type = params.tracker;

    threshold_intersection = params.threshold_intersection;
    remove_duplicates =
        params.remove_duplicates && (traversal_mode == scribo::SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::HORIZONTAL_VERTICAL);
    this->min_length = min_length;
  }
} // namespace scribo::internal
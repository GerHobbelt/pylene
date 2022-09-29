#include <scribo/segdet.hpp>

namespace scribo
{
  bool SegDetParams::is_valid() const
  {
    if (nb_values_to_keep < minimum_for_fusion)
      return false;

    if (max_llum > max_max_llum)
      return false;

    if (preprocess != SEGDET_PREPROCESS_ENUM::NONE && preprocess != SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT)
      return false;

    if (traversal_mode != SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::HORIZONTAL &&
        traversal_mode != SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL &&
        traversal_mode != SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::HORIZONTAL_VERTICAL)
      return false;

    if (tracker != SEGDET_PROCESS_TRACKING_ENUM::KALMAN && tracker != SEGDET_PROCESS_TRACKING_ENUM::KALMAN_LEPLUMEY &&
        tracker != SEGDET_PROCESS_TRACKING_ENUM::ONE_EURO &&
        tracker != SEGDET_PROCESS_TRACKING_ENUM::DOUBLE_EXPONENTIAL &&
        tracker != SEGDET_PROCESS_TRACKING_ENUM::LAST_INTEGRATION && 
        tracker != SEGDET_PROCESS_TRACKING_ENUM::SIMPLE_MOVING_AVERAGE &&
        tracker != SEGDET_PROCESS_TRACKING_ENUM::EXPONENTIAL_MOVING_AVERAGE)
      return false;

    if (bucket_size == 0)
      return false;

    return true;
  }
} // namespace scribo
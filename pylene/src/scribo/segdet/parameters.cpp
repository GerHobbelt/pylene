#include <scribo/segdet.hpp>

namespace scribo
{
  bool SegDetParams::is_valid() const
  {
    if (nb_values_to_keep < minimum_for_fusion)
      return false;

    if (llumi > blumi)
      return false;

    if (preprocess != e_segdet_preprocess::NONE && preprocess != e_segdet_preprocess::BLACK_TOP_HAT)
      return false;

    if (traversal_mode != e_segdet_process_traversal_mode::HORIZONTAL &&
        traversal_mode != e_segdet_process_traversal_mode::VERTICAL &&
        traversal_mode != e_segdet_process_traversal_mode::HORIZONTAL_VERTICAL)
      return false;

    if (tracker != e_segdet_process_tracking::KALMAN && tracker != e_segdet_process_tracking::ONE_EURO &&
        tracker != e_segdet_process_tracking::DOUBLE_EXPONENTIAL &&
        tracker != e_segdet_process_tracking::LAST_INTEGRATION &&
        tracker != e_segdet_process_tracking::SIMPLE_MOVING_AVERAGE &&
        tracker != e_segdet_process_tracking::EXPONENTIAL_MOVING_AVERAGE)
      return false;

    if (bucket_size == 0)
      return false;

    return true;
  }
} // namespace scribo
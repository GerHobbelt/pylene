
#include <scribo/segdet.hpp>

namespace scribo
{
  bool SegDetParams::is_valid() const { return nb_values_to_keep > minimum_for_fusion; }
} // namespace mln::contrib::segdet
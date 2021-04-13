#pragma once
#include <mln/contrib/segdet/segdet.hpp>

namespace pln::contrib::segdet
{
  int detect_line_hat(mln::ndbuffer_image img, int min_len, int disc)
  {
    auto p = mln::contrib::segdet::detect_line(img.__cast<uint8_t, 2>(), min_len, disc);

    return p.size();
  }
} // namespace pln::contri::segdet
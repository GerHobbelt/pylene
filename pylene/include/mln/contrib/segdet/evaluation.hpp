#pragma once

#include <vector>
#include <mln/core/image/ndimage.hpp>

namespace mln::contrib::segdet
{
  std::vector<double> score_detection(mln::ndbuffer_image img_ref, mln::ndbuffer_image output);
}
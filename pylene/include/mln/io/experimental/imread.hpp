#pragma once

#include <mln/core/image/experimental/ndimage_fwd.hpp>
#include <string>


namespace mln::io::experimental
{
  mln::ndbuffer_image imread(const std::string& filename);
  void                imread(const std::string& filename, mln::ndbuffer_image& out);
}

#pragma once

#include <ano/core/image/ndimage_fwd.hpp>
#include <string>

namespace ano::io::fits
{
  ano::ndbuffer_image imread(const std::string& filename, int ind = 0);
  void                imread(const std::string& filename, ano::ndbuffer_image& out, int ind = 0);
} // namespace ano::io::fits
#pragma once

#include <ano/core/image/ndimage_fwd.hpp>
#include <string>


namespace ano::io::oiio
{
  ano::ndbuffer_image imread(const std::string& filename);
  void                imread(const std::string& filename, ano::ndbuffer_image& out);

} // namespace ano::io::oiio

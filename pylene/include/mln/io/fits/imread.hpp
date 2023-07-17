#pragma once

#include <mln/core/image/ndimage_fwd.hpp>
#include <string>

namespace mln::io::fits
{
    mln::ndbuffer_image imread(const std::string& filename, int ind=0);
    void                imread(const std::string& filename, mln::ndbuffer_image& out, int ind=0);
}
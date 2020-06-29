#pragma once

#include <mln/core/image/ndimage_fwd.hpp>

#include <pybind11/pybind11.h>


namespace mln::py
{

  mln::ndbuffer_image   ndimage_from_buffer(pybind11::buffer b);
  pybind11::buffer_info ndimage_to_buffer(const mln::ndbuffer_image& img);

} // namespace mln::py

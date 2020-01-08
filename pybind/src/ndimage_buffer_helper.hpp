#pragma once

#include <mln/core/image/experimental/ndimage_fwd.hpp>

#include <pybind11/pybind11.h>


namespace mln::py
{

  void                  ndimage_from_buffer(mln::ndbuffer_image& m, pybind11::buffer b);
  pybind11::buffer_info ndimage_to_buffer(const mln::ndbuffer_image& img);

} // namespace mln::py

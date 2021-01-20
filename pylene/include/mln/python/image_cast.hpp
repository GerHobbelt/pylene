#pragma once

#include <mln/core/image/ndbuffer_image.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace pln
{
  mln::ndbuffer_image from_numpy(pybind11::array arr);

  pybind11::array to_numpy(mln::ndbuffer_image);
} // namespace pln

namespace pybind11::detail
{
  template <>
  struct type_caster<mln::ndbuffer_image>
  {
    PYBIND11_TYPE_CASTER(mln::ndbuffer_image, _("numpy.ndarray"));

    bool load(handle h, bool)
    {
      pybind11::array arr = reinterpret_borrow<pybind11::array>(h);
      value               = pln::from_numpy(arr);
      return true;
    }

    static handle cast(mln::ndbuffer_image img, return_value_policy, handle) { return pln::to_numpy(img).inc_ref(); }
  };
} // namespace pybind11::detail
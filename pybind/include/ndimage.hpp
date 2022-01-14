#pragma once

#include <mln/core/image/ndbuffer_image.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>


namespace mln::py
{
  mln::ndbuffer_image from_numpy(pybind11::array arr);

  pybind11::object to_numpy(const mln::ndbuffer_image& img);

  void init_pylena_numpy(pybind11::module& m);
} // namespace mln::py

namespace pybind11::detail
{
  template <>
  struct type_caster<mln::ndbuffer_image>
  {
    PYBIND11_TYPE_CASTER(mln::ndbuffer_image, _("numpy.ndarray"));

    bool load(handle h, bool)
    {
      pybind11::array arr = reinterpret_borrow<pybind11::array>(h);
      value               = mln::py::from_numpy(arr);
      return true;
    }

    static handle cast(const mln::ndbuffer_image& img, return_value_policy, handle)
    {
      return mln::py::to_numpy(img).inc_ref();
    }
  };
} // namespace pybind11::detail

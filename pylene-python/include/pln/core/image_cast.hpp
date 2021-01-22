#pragma once

#include <mln/core/image/ndbuffer_image.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace pln
{
  /// \brief Convert a NumPy array to a ndbuffer_image
  /// \param[in] arr A NumPy array
  /// \return A ndbuffer_image
  mln::ndbuffer_image from_numpy(pybind11::array arr);

  /// \brief Convert a ndbuffer_image array to a NumPy
  /// \param[in] arr A ndbuffer_image
  /// \return A NumPy array
  pybind11::object to_numpy(mln::ndbuffer_image img);
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
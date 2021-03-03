#pragma once

#include <mln/core/image_format.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace pln
{
  /// \brief Conversion from enumerated type to NumPy string type
  /// \param[in] type An enumerated type
  /// \return A string corresponding to the NumPy type
  pybind11::dtype get_sample_type(mln::sample_type_id type);

  /// \brief Conversion from NumPy string type to enumerated type
  /// \param[in] type A string corresponding to the NumPy type
  /// \return An enumerated type
  mln::sample_type_id get_sample_type(pybind11::dtype type);
} // namespace pln
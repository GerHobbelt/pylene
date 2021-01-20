#pragma once

#include <mln/core/image_format.hpp>

#include <pybind11/pybind11.h>

namespace pln
{
  /* mln -> numpy sample type */
  std::string get_sample_type(mln::sample_type_id);

  /* numpy -> mln sample type */
  mln::sample_type_id get_sample_type(const std::string&);
} // namespace pln
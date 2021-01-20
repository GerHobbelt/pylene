#pragma once

#include <mln/morpho/tos.hpp>
#include <mln/python/image_cast.hpp>

namespace pln
{
  auto tos(const mln::ndbuffer_image& img);

  void def_morpho_module(pybind11::module_& m);
} // namespace pln
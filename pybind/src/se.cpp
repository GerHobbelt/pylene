#include "se.hpp"

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;


void init_module_se(::py::module& m)
{
  ::py::class_<mln::experimental::se::disc_non_decomp>(m, "disc").def(
      ::py::init([](float radius) { return mln::experimental::se::disc_non_decomp{radius}; }));

  ::py::class_<mln::experimental::se::disc>(m, "disc_decomposable").def(::py::init([](float radius) {
    return mln::experimental::se::disc{radius};
  }));

  ::py::class_<mln::experimental::se::rect2d_non_decomp>(m, "rect2d").def(::py::init([](int width, int height) {
    return mln::experimental::se::rect2d_non_decomp{width, height};
  }));

  ::py::class_<mln::experimental::se::rect2d>(m, "rect2d_decomposable").def(::py::init([](int width, int height) {
    return mln::experimental::se::rect2d{width, height};
  }));
}

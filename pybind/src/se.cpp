#include "se.hpp"

#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <pybind11/pybind11.h>


namespace mln::py
{

  void init_module_se(pybind11::module& m)
  {
    pybind11::class_<mln::se::disc_non_decomp>(m, "disc").def(
        pybind11::init([](float radius) { return mln::se::disc_non_decomp{radius}; }));

    pybind11::class_<mln::se::disc>(m, "disc_decomposable").def(pybind11::init([](float radius) {
      return mln::se::disc{radius};
    }));

    pybind11::class_<mln::se::rect2d_non_decomp>(m, "rect2d").def(pybind11::init([](int width, int height) {
      return mln::se::rect2d_non_decomp{width, height};
    }));

    pybind11::class_<mln::se::rect2d>(m, "rect2d_decomposable").def(pybind11::init([](int width, int height) {
      return mln::se::rect2d{width, height};
    }));
  }

} // namespace mln::py

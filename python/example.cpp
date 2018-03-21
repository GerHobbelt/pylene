#include <pybind11/pybind11.h>
#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/colors.hpp>

namespace py = pybind11;


std::unique_ptr<mln::NDImageInterface> create_iota(int width, int height)
{
  auto f = std::make_unique<mln::image2d<mln::uint8>>(height, width);
  mln::iota(*f, 1);

  return f;
}

std::unique_ptr<mln::NDImageInterface> create_iota2(int width, int height)
{
  auto f = std::make_unique<mln::image2d<mln::rgb<mln::uint16>>>(height, width);
  mln::iota(mln::red(*f), 1);
  mln::iota(mln::green(*f), 1<<6);
  mln::iota(mln::blue(*f), 1<<10);

  return f;
}

void bind_ndimage(py::module& m);


PYBIND11_MODULE(pylene, m) {
  m.def("create_iota", &create_iota, "An example.");
  m.def("create_iota2", &create_iota2, "An example.");
  bind_ndimage(m);
}

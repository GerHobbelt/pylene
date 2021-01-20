#include <mln/core/image/private/ndbuffer_image_data.hpp>

#include <pln/core/image_cast.hpp>

#include <pybind11/pybind11.h>

#include <memory>

namespace pln
{
  namespace py = pybind11;

  PYBIND11_MODULE(pylena, m)
  {
    py::class_<mln::internal::ndbuffer_image_data, std::shared_ptr<mln::internal::ndbuffer_image_data>>(
        m, "ndbuffer_image_data");
    m.def("id", [](const mln::ndbuffer_image& img) { return img; });
  }
} // namespace pln
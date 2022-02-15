#include <pln/core/image_cast.hpp>

#include "segdet.hpp"

namespace pln::scribo
{
  void define_scribo(pybind11::module& _m)
  {
    auto m = _m.def_submodule("scribo");
    m.def("detect_line_label", &pln::scribo::detect_line_label, pybind11::arg("img"), pybind11::arg("min_len"),
          pybind11::arg("params") = std::map<std::string, float>());

    m.def("detect_line", &pln::scribo::detect_line, pybind11::arg("img"), pybind11::arg("min_len"),
          pybind11::arg("mode") = "pixel", pybind11::arg("params") = std::map<std::string, float>());
  }
} // namespace pln::scribo
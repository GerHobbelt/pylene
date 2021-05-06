#include <pln/contrib/segdet.hpp>
#include <pln/core/image_cast.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

#include <mln/core/image_format.hpp>

namespace pln
{

  namespace py = pybind11;

  PYBIND11_MODULE(pylena, m)
  {
    init_pylena_numpy(m);
    m.def("detect_line_se", &pln::contrib::segdet::detect_line_sorry_edwin,
          py::arg("img"),
          py::arg("min_len"), py::arg("disc"),
          py::arg("params") = std::map<std::string, double>());
    m.def("detect_line_l", &pln::contrib::segdet::detect_line_labelled,
          py::arg("img"),
          py::arg("min_len"), py::arg("disc"),
          py::arg("params") = std::map<std::string, double>());

  }
} // namespace pln
#include <pln/contrib/segdet.hpp>
#include <mln/contrib/segdet/evaluation.hpp>
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

    m.def("detect_line_l", &pln::contrib::segdet::detect_line_labelled, py::arg("img"), py::arg("min_len"),
          py::arg("disc"), py::arg("params") = std::map<std::string, double>());
    
    m.def("detect_line_np", &pln::contrib::segdet::detect_line_numpy, py::arg("img"), py::arg("min_len"),
          py::arg("disc"), py::arg("params") = std::map<std::string, double>());

    m.def("detect_line_vectorize", &pln::contrib::segdet::detect_line_vectorize, py::arg("img"), py::arg("min_len"),
          py::arg("disc"), py::arg("params") = std::map<std::string, double>(), py::arg("precision") = 15);

    m.def("score_detection", &mln::contrib::segdet::score_detection);
  }
} // namespace pln
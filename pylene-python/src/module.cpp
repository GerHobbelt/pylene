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
    m.def("detect_line", &pln::contrib::segdet::detect_line_hat);
    m.def("preprocess", &pln::contrib::segdet::preprocess);
  }
} // namespace pln
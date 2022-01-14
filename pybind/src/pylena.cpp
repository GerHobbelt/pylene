#include "morpho.hpp"
#include "ndimage.hpp"
#include "se.hpp"

#include <pybind11/pybind11.h>


PYBIND11_MODULE(pylena, m)
{
  mln::py::init_pylena_numpy(m);

  auto mmorpho = m.def_submodule("morpho", "Mathematical morphology module.");
  mln::py::init_module_morpho(mmorpho);

  auto mse = m.def_submodule("se", "Structuring elements module.");
  mln::py::init_module_se(mse);
}

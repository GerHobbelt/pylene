#include "morpho.hpp"
#include "ndimage.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;


PYBIND11_MODULE(Pylena, m)
{
  init_class_ndimage(m);

  auto mm = m.def_submodule("morpho", "Mathematical morphology module.");
  init_module_morpho(mm);
}

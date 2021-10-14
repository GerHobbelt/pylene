#include <pln/core/image_cast.hpp>
#include <pln/morpho/morpho.hpp>

#include <pybind11/pybind11.h>

namespace pln
{

  PYBIND11_MODULE(pylena, m)
  {
    init_pylena_numpy(m);

    {
      auto mm = m.def_submodule("morpho", "Mathematical morphology module");
      init_morpho_module(mm);
    }
  }
} // namespace pln

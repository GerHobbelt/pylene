#pragma once

#include <pybind11/pybind11.h>


namespace mln::py
{

  void init_module_morpho(pybind11::module& m);

}

#pragma once

#include <pybind11/pybind11.h>

namespace pln::morpho
{
    void define_morpho_module(pybind11::module_& m);
}
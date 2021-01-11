#pragma once

#include <mln/python/image_cast.hpp>

#include <string>

namespace pln
{
    void imsave(const std::string& filename, const mln::ndbuffer_image& img);

    void def_io_module(pybind11::module_& m);
}
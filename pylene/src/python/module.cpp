#include <mln/core/image/private/ndbuffer_image_data.hpp>
#include <mln/python/image_cast.hpp>
#include <mln/python/io.hpp>
#include <mln/python/utils.hpp>

#include <pybind11/pybind11.h>

#include <memory>

namespace pln
{
    namespace py = pybind11;

    PYBIND11_MODULE(pylena, m)
    {
        py::class_<mln::internal::ndbuffer_image_data, std::shared_ptr<mln::internal::ndbuffer_image_data>>(m, "ndbuffer_image_data");
        def_io_module(m);
        def_utils_module(m);
    }
}
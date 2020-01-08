#include "ndimage.hpp"

#include "ndimage_buffer_helper.hpp"

#include <mln/core/image/experimental/ndimage.hpp>


namespace py = pybind11;


void init_class_ndimage(pybind11::module& m)
{
  py::class_<mln::ndbuffer_image>(m, "ndimage", py::buffer_protocol()) //
      .def("__init__", mln::py::ndimage_from_buffer)
      .def_buffer(mln::py::ndimage_to_buffer);
}

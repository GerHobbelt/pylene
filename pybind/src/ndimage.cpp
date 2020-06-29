#include "ndimage.hpp"

#include "ndimage_buffer_helper.hpp"

#include <mln/core/image/ndimage.hpp>


namespace py = pybind11;


void init_class_ndimage(py::module& m)
{
  py::class_<mln::ndbuffer_image>(m, "ndimage", py::buffer_protocol()) //
      .def(py::init([](py::buffer b) { return mln::py::ndimage_from_buffer(b); }))
      .def_buffer(mln::py::ndimage_to_buffer);
}

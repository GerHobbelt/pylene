#include <py/algorithm/stretch.hpp>
#include <py/core/image2d.hpp>
#include <py/core/type_info.hpp>
#include <pybind/numpy_helper.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <cstdint>
#include <string>


namespace py = pybind11;

/*mln::py::Info::type_id get_typeid_from_buffer_mln::py::Info(py::buffer_mln::py::Info mln::py::Info)
{
    switch (mln::py::Info)
    {
        case
        default: return mln::py::Info::OTHER_V;
    }
}
*/


PYBIND11_MODULE(PyPylene, m)
{
  using namespace mln::py;
  py::class_<image2d<>>(m, "image2d", py::buffer_protocol())
      .def("__init__",
           [](image2d<>& img, py::buffer b) {
             /* Request a buffer descriptor from Python */
             py::buffer_info info = b.request();

             /* Sanity checks */
             if (info.format != py::format_descriptor<int8_t>::format())
               throw std::runtime_error("Incompatible format: got " + info.format);
             if (info.ndim != 2)
               throw std::runtime_error("Incompatible buffer dimension!");

             auto tmp = image2d<void>::from_buffer(reinterpret_cast<std::byte*>(info.ptr), info.shape[0], info.shape[1],
                                                   Info::INT8_V);
             new (&img) image2d<void>(tmp);
           })

      .def_buffer([](image2d<void>& img) -> py::buffer_info {
        return py::buffer_info(img.buffer(), Info::dyn_sizeof[img.type().val], sampletypeid_to_numpystr(img.type().val),
                               2, {img.height(), img.width()},
                               {Info::dyn_sizeof[img.type().val] * img.width(), Info::dyn_sizeof[img.type().val]});
      });
  // FIXME:
  // m.def("stretch", static_cast <image2d <> (*)(const image2d <>&)>(&mln::py::stretch));
}

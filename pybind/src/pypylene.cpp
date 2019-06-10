#include <py/algorithm/stretch.hpp>
#include <py/algorithm/invert.hpp>
#include <py/core/image2d.hpp>
#include <py/core/type_info.hpp>
#include <pybind/numpy_helper.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <cstdint>
#include <string>


namespace py = pybind11;


py::array to_numpy(const mln::py::image2d<>& ima)
{
  using namespace mln::py;

  auto info = py::buffer_info{ima.buffer(),
                              static_cast<ssize_t>(Info::dyn_sizeof(ima.type().tid())),
                              sampletypeid_to_numpystr(ima.type().tid()),
                              2,
                              {ima.height(), ima.width()},
                              {Info::dyn_sizeof(ima.type().tid()) * ima.width(), Info::dyn_sizeof(ima.type().tid())}

  };
  return py::array{info};
}

PYBIND11_MODULE(PyPylene, m)
{
  using namespace mln::py;
  py::class_<image2d<>>(m, "image2d", py::buffer_protocol())
      .def("__init__",
           [](image2d<>& img, py::buffer b) {
             /* Request a buffer descriptor from Python */
             py::buffer_info info = b.request();

             /* Sanity checks */
             if (info.ndim != 2)
               throw std::runtime_error("Incompatible buffer dimension!");

             auto tmp = image2d<void>::from_buffer(reinterpret_cast<std::byte*>(info.ptr), info.shape[0], info.shape[1],
                                                   numpystr_to_sampletypeid(info.format));
             new (&img) image2d<void>(tmp);
           })

      .def_buffer([](image2d<void>& img) -> py::buffer_info {
        return py::buffer_info(img.buffer(), Info::dyn_sizeof(img.type().tid()),
                               sampletypeid_to_numpystr(img.type().tid()), 2, {img.height(), img.width()},
                               {Info::dyn_sizeof(img.type().tid()) * img.width(), Info::dyn_sizeof(img.type().tid())});
      });

  m.def("stretch", static_cast<image2d<> (*)(const image2d<>&)>(&mln::py::stretch_py));
  m.def("invert", static_cast<image2d<> (*)(const image2d<>&)>(&mln::py::invert_py));
  m.def("to_numpy", &to_numpy);
}

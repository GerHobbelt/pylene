#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "../ima.hh"

namespace py = pybind11;

std::string sampletypeid_to_numpystr(Info::type_id sample_type)
{
    switch (sample_type)
    {
        case Info::UINT8_V: return py::format_descriptor<uint8_t>::format();
        case Info::INT8_V: return py::format_descriptor<int8_t>::format();
        case Info::UINT16_V: return py::format_descriptor<uint16_t>::format();
        case Info::INT16_V: return py::format_descriptor<int16_t>::format();
        case Info::UINT32_V: return py::format_descriptor<uint32_t>::format();
        case Info::INT32_V: return py::format_descriptor<int32_t>::format();
        case Info::UINT64_V: return py::format_descriptor<uint64_t>::format();
        case Info::INT64_V: return py::format_descriptor<int64_t>::format();
        case Info::FLOAT_V: return py::format_descriptor<float>::format();
        case Info::DOUBLE_V: return py::format_descriptor<double>::format();
        default: return "";
    }
}

/*Info::type_id get_typeid_from_buffer_info(py::buffer_info info)
{
    switch (info)
    {
        case
        default: return Info::OTHER_V;
    }
}
*/


PYBIND11_MODULE(milena, m)
{
    py::class_<image2d<>>(m, "image2d", py::buffer_protocol())
        .def("__init__", [](image2d<> &img, py::buffer b)
            {
                /* Request a buffer descriptor from Python */
                py::buffer_info info = b.request();

                /* Sanity checks */
                if (info.format != py::format_descriptor<int8_t>::format())
                    throw std::runtime_error("Incompatible format: got " + info.format);
                if (info.ndim != 2)
                    throw std::runtime_error("Incompatible buffer dimension!");

                auto tmp = image2d<void>::from_buffer(
                        reinterpret_cast<std::byte*>(info.ptr),
                        info.shape[0],
                        info.shape[1],
                        Info::INT8_V);
                new (&img) image2d<void>(tmp);
            })

        .def_buffer([](image2d<void>& img) -> py::buffer_info
            {
                return py::buffer_info(
                        img.buffer(),
                        Info::dyn_sizeof[img.type().val],
                        sampletypeid_to_numpystr(img.type().val),
                        2,
                        { img.height(), img.width() },
                        { Info::dyn_sizeof[img.type().val] * img.width(),
                        Info::dyn_sizeof[img.type().val] }
                );
            });

    m.def("stretch", static_cast<image2d<>(*)(const image2d<>&)>(&stretch));
}

#include <pybind/numpy_helper.hpp>

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

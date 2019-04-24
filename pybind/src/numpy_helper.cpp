#include <pybind/numpy_helper.hpp>

#include <pybind11/pybind11.h>

using namespace mln::py;
namespace py = pybind11;

std::string sampletypeid_to_numpystr(Info::type_id sample_type)
{
  switch (sample_type)
  {
  case Info::UINT8_V:
    return py::format_descriptor<uint8_t>::format();
  case Info::INT8_V:
    return py::format_descriptor<int8_t>::format();
  case Info::UINT16_V:
    return py::format_descriptor<uint16_t>::format();
  case Info::INT16_V:
    return py::format_descriptor<int16_t>::format();
  case Info::UINT32_V:
    return py::format_descriptor<uint32_t>::format();
  case Info::INT32_V:
    return py::format_descriptor<int32_t>::format();
  case Info::UINT64_V:
    return py::format_descriptor<uint64_t>::format();
  case Info::INT64_V:
    return py::format_descriptor<int64_t>::format();
  case Info::FLOAT_V:
    return py::format_descriptor<float>::format();
  case Info::DOUBLE_V:
    return py::format_descriptor<double>::format();
  default:
    return "";
  }
}

Info::type_id numpystr_to_sampletypeid(const std::string &s)
{
    static Info::type_id converter[256];
    converter['c'] = Info::INT8_V;
    converter['b'] = Info::INT8_V;
    converter['B'] = Info::UINT8_V;
    converter['h'] = Info::INT16_V;
    converter['H'] = Info::UINT16_V;
    converter['i'] = Info::INT32_V;
    converter['I'] = Info::UINT32_V;
    converter['l'] = Info::INT64_V;
    converter['L'] = Info::UINT64_V;
    converter['f'] = Info::FLOAT_V;
    converter['d'] = Info::DOUBLE_V;
    return converter[static_cast<int>(s[0])];
}

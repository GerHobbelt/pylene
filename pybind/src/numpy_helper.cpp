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

Info::type_id numpystr_to_sampletypeid(const std::string& s)
{
  static Info::type_id converter[256];
  using uch           = unsigned char;
  converter[(uch)'c'] = Info::INT8_V;
  converter[(uch)'b'] = Info::INT8_V;
  converter[(uch)'B'] = Info::UINT8_V;
  converter[(uch)'h'] = Info::INT16_V;
  converter[(uch)'H'] = Info::UINT16_V;
  converter[(uch)'i'] = Info::INT32_V;
  converter[(uch)'I'] = Info::UINT32_V;
  converter[(uch)'l'] = Info::INT64_V;
  converter[(uch)'L'] = Info::UINT64_V;
  converter[(uch)'f'] = Info::FLOAT_V;
  converter[(uch)'d'] = Info::DOUBLE_V;
  return converter[static_cast<Info::type_id>(s[0])];
}

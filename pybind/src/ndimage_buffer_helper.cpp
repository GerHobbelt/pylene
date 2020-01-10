#include "ndimage_buffer_helper.hpp"

#include "visit.hpp"

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/trace.hpp>

#include <string>


namespace py = pybind11;

namespace
{

  template <typename T>
  struct format_descriptor_t
  {
    std::string operator()() const { return py::format_descriptor<T>::format(); }
  };

  template <>
  struct format_descriptor_t<mln::rgb8>
  {
    std::string operator()() const { return ""; }
  };

  // Python sample type -> pylene sample type
  std::string get_sample_type(mln::sample_type_id tid)
  {
    return mln::py::visit_d<format_descriptor_t>(tid, std::string{""});
  }


  mln::sample_type_id get_sample_type(const std::string& s)
  {
    // Scalar type are one letter long
    if (s.size() != 1)
      return mln::sample_type_id::OTHER;

    char c = s[0];
    // clang-format off
    switch (c)
    {
      case ::py::format_descriptor<bool>::value[0]          : return mln::sample_type_id::BOOL       ;
      case ::py::format_descriptor<std::uint8_t>::value[0]  : return mln::sample_type_id::UINT8      ;
      case ::py::format_descriptor<std::uint16_t>::value[0] : return mln::sample_type_id::UINT16     ;
      case ::py::format_descriptor<std::uint32_t>::value[0] : return mln::sample_type_id::UINT32     ;
      case ::py::format_descriptor<std::uint64_t>::value[0] : return mln::sample_type_id::UINT64     ;
      case ::py::format_descriptor<std::int8_t>::value[0]   : return mln::sample_type_id::INT8       ;
      case ::py::format_descriptor<std::int16_t>::value[0]  : return mln::sample_type_id::INT16      ;
      case ::py::format_descriptor<std::int32_t>::value[0]  : return mln::sample_type_id::INT32      ;
      case ::py::format_descriptor<std::int64_t>::value[0]  : return mln::sample_type_id::INT64      ;
      case ::py::format_descriptor<float>::value[0]         : return mln::sample_type_id::FLOAT      ;
      case ::py::format_descriptor<double>::value[0]        : return mln::sample_type_id::DOUBLE     ;
      default: return mln::sample_type_id::OTHER;
    }
    // clang-format on
  }
} // namespace

namespace mln::py
{
  mln::ndbuffer_image ndimage_from_buffer(::py::buffer b)
  {
    mln_entering("test");

    /* Request a buffer descriptor from Python */
    ::py::buffer_info info = b.request();

    std::ptrdiff_t      strides[16];
    int                 dims[16];
    int                 ndim = info.ndim;
    int                 bpp  = info.itemsize;
    mln::sample_type_id st   = get_sample_type(info.format);

    for (int i = 0; i < ndim; ++i)
    {
      dims[i]    = info.shape[ndim - 1 - i];
      strides[i] = info.strides[ndim - 1 - i];
    }

    if (strides[0] != bpp)
    {
      throw std::runtime_error("Unsupported image stride along the last dimension.");
    }

    return mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(info.ptr), st, ndim, dims, strides);
  }


  ::py::buffer_info ndimage_to_buffer(const mln::ndbuffer_image& img)
  {
    auto ti = get_sample_type_id_traits(img.sample_type());

    int ndim = img.pdim();

    std::vector<ssize_t> dims(ndim);
    std::vector<ssize_t> strides(ndim);

    for (int i = 0; i < ndim; ++i)
    {
      dims[i]    = img.size(ndim - 1 - i);
      strides[i] = img.byte_stride(ndim - 1 - i);
    }

    return ::py::buffer_info(img.buffer(), ti.size(), get_sample_type(img.sample_type()), ndim, dims, strides);
  }
} // namespace mln::py

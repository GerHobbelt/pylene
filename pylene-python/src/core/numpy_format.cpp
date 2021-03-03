#include <pln/core/numpy_format.hpp>

namespace pln
{
  namespace details
  {
    //template <mln::sample_type_id t>
    //using numpy_desc = pybind11::format_descriptor<typename mln::sample_type_id_traits<t>::type>;
  }

  pybind11::dtype get_sample_type(mln::sample_type_id type)
  {
    switch (type)
    {
    case mln::sample_type_id::INT8:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT8>::type>();
    case mln::sample_type_id::INT16:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT16>::type>();
    case mln::sample_type_id::INT32:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT32>::type>();
    case mln::sample_type_id::INT64:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT64>::type>();
    case mln::sample_type_id::UINT8:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT8>::type>();
    case mln::sample_type_id::UINT16:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT16>::type>();
    case mln::sample_type_id::UINT32:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT32>::type>();
    case mln::sample_type_id::UINT64:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT64>::type>();
    case mln::sample_type_id::FLOAT:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::FLOAT>::type>();
    case mln::sample_type_id::DOUBLE:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::DOUBLE>::type>();
    case mln::sample_type_id::BOOL:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::BOOL>::type>();
    case mln::sample_type_id::RGB8:
      return pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT8>::type>();
    default:
      return pybind11::none();
    }
  }

  mln::sample_type_id get_sample_type(pybind11::dtype type)
  {
    if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT8>::type>().ptr() == type.ptr())
      return mln::sample_type_id::INT8;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT16>::type>().ptr() == type.ptr())
      return mln::sample_type_id::INT16;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT32>::type>().ptr() == type.ptr())
      return mln::sample_type_id::INT32;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::INT64>::type>().ptr() == type.ptr())
      return mln::sample_type_id::INT64;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT8>::type>().ptr() == type.ptr())
      return mln::sample_type_id::UINT8;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT16>::type>().ptr() == type.ptr())
      return mln::sample_type_id::UINT16;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT32>::type>().ptr() == type.ptr())
      return mln::sample_type_id::UINT32;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::UINT64>::type>().ptr() == type.ptr())
      return mln::sample_type_id::UINT64;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::FLOAT>::type>().ptr() == type.ptr())
      return mln::sample_type_id::FLOAT;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::DOUBLE>::type>().ptr() == type.ptr())
      return mln::sample_type_id::DOUBLE;
    else if (pybind11::dtype::of<mln::sample_type_id_traits<mln::sample_type_id::BOOL>::type>().ptr() == type.ptr())
      return mln::sample_type_id::BOOL;
    return mln::sample_type_id::OTHER;
  }
} // namespace pln
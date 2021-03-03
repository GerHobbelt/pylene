#include <mln/core/image/private/ndbuffer_image_data.hpp>
#include <pln/core/image_cast.hpp>
#include <pln/core/numpy_format.hpp>

#include <pybind11/cast.h>

#include <vector>

namespace pln
{
  mln::ndbuffer_image from_numpy(pybind11::array arr)
  {
    auto       base    = arr.base();
    const auto info    = arr.request();
    const auto type    = get_sample_type(pybind11::dtype(info.format));
    const bool is_rgb8 = info.ndim == 3 && info.shape[2] == 3 && type == mln::sample_type_id::UINT8;
    const auto pdim    = info.ndim - (is_rgb8 ? 1 : 0);
    if (pdim > mln::PYLENE_NDBUFFER_DEFAULT_DIM)
      throw std::invalid_argument("Invalid number of dimension (should be < 5)");
    int            size[mln::PYLENE_NDBUFFER_DEFAULT_DIM]    = {0};
    std::ptrdiff_t strides[mln::PYLENE_NDBUFFER_DEFAULT_DIM] = {0};
    for (auto d = 0; d < pdim; d++)
    {
      size[d]    = info.shape[pdim - d - 1];
      strides[d] = info.strides[pdim - d - 1];
      if (d > 0 && strides[d] < strides[d - 1])
        throw std::invalid_argument("Array should be C contiguous");
    }
    const auto sample_type = is_rgb8 ? mln::sample_type_id::RGB8 : type;
    auto       res =
        mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(info.ptr), sample_type, pdim, size, strides);
    if (base && pybind11::isinstance<mln::internal::ndbuffer_image_data>(base))
      res.__data() = pybind11::cast<std::shared_ptr<mln::internal::ndbuffer_image_data>>(base);
    return res;
  }

  pybind11::object to_numpy(mln::ndbuffer_image img)
  {
    auto& api   = pybind11::detail::npy_api::get();
    auto  data  = pybind11::handle();
    int   flags = ~pybind11::detail::npy_api::NPY_ARRAY_OWNDATA_ & pybind11::detail::npy_api::NPY_ARRAY_WRITEABLE_;
    if (img.__data())
    {
      data = pybind11::cast(img.__data()).inc_ref();
      flags &= pybind11::detail::npy_api::NPY_ARRAY_C_CONTIGUOUS_;
    }

    /* For the moment, restrict RGB8 image to 2D image */
    const bool               is_rgb8 = img.pdim() == 2 && img.sample_type() == mln::sample_type_id::RGB8;
    const auto               ndim    = img.pdim() + (is_rgb8 ? 1 : 0);
    std::vector<std::size_t> strides(ndim, 1);
    std::vector<std::size_t> shapes(ndim, 3);
    auto                     descr = get_sample_type(img.sample_type());

    for (auto d = 0; d < img.pdim(); d++)
    {
      strides[d] = img.byte_stride(img.pdim() - d - 1);
      shapes[d]  = img.size(img.pdim() - d - 1);
    }

    auto res = pybind11::reinterpret_steal<pybind11::object>(api.PyArray_NewFromDescr_(
        api.PyArray_Type_, descr.release().ptr(), ndim,
        reinterpret_cast<Py_intptr_t*>(shapes.data()), reinterpret_cast<Py_intptr_t*>(strides.data()),
        reinterpret_cast<void*>(img.buffer()), flags, nullptr));

    if (!res)
      throw std::runtime_error("Unable to create the numpy array in ndimage -> array");
    if (data)
      api.PyArray_SetBaseObject_(res.ptr(), data.inc_ref().ptr());
    return res;
  }

  void init_pylena_numpy(pybind11::module& m)
  {
    if (!pybind11::detail::get_global_type_info(typeid(mln::internal::ndbuffer_image_data)) &&
        !pybind11::detail::get_global_type_info(typeid(std::shared_ptr<mln::internal::ndbuffer_image_data>)))
    {
      pybind11::class_<mln::internal::ndbuffer_image_data, std::shared_ptr<mln::internal::ndbuffer_image_data>>(
          m, "ndbuffer_image_data");
    }
  }
} // namespace pln
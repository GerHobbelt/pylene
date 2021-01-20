#include <mln/core/image/private/ndbuffer_image_data.hpp>
#include <mln/python/image_cast.hpp>
#include <mln/python/numpy_format.hpp>

#include <vector>

namespace pln
{
  mln::ndbuffer_image from_numpy(pybind11::array arr)
  {
    auto                        base    = arr.base();
    const auto                  info    = arr.request();
    const auto                  type    = get_sample_type(info.format);
    const bool                  is_rgb8 = info.ndim == 3 && info.shape[2] == 3 && type == mln::sample_type_id::UINT8;
    const auto                  pdim    = info.ndim - (is_rgb8 ? 1 : 0);
    std::vector<int>            size(pdim);
    std::vector<std::ptrdiff_t> strides(pdim);
    for (auto d = 0; d < pdim; d++)
    {
      size[d]    = info.shape[pdim - d - 1];
      strides[d] = info.strides[pdim - d - 1];
    }
    const auto sample_type = is_rgb8 ? mln::sample_type_id::RGB8 : type;
    auto res = mln::ndbuffer_image::from_buffer(reinterpret_cast<std::byte*>(info.ptr), sample_type, pdim, size.data(),
                                                strides.data());
    if (base && pybind11::isinstance<mln::internal::ndbuffer_image_data>(base))
      res.__data() = pybind11::cast<std::shared_ptr<mln::internal::ndbuffer_image_data>>(base);
    return res;
  }

  pybind11::array to_numpy(mln::ndbuffer_image img)
  {
    auto data = pybind11::handle();
    if (img.__data())
      data = pybind11::cast(img.__data()).inc_ref();

    /* For the moment, restrict RGB8 image to 2D image */
    const bool               is_rgb8 = img.pdim() == 2 && img.sample_type() == mln::sample_type_id::RGB8;
    const auto               ndim    = img.pdim() + (is_rgb8 ? 1 : 0);
    std::vector<std::size_t> strides(ndim, 1);
    std::vector<std::size_t> shapes(ndim, 3);

    for (auto d = 0; d < img.pdim(); d++)
    {
      strides[d] = img.byte_stride(img.pdim() - d - 1);
      shapes[d]  = img.size(img.pdim() - d - 1);
    }

    return pybind11::array(pybind11::buffer_info(img.buffer(), mln::get_sample_type_id_traits(img.sample_type()).size(),
                                                 get_sample_type(img.sample_type()), ndim, shapes, strides),
                           data /* To keep data alive until the end of the life of the numpy array*/);
  }
} // namespace pln
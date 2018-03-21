#ifndef MLN_CORE_WRT_OFFSET_HPP
#define MLN_CORE_WRT_OFFSET_HPP

#include <array>
#include <mln/core/image/image.hpp>
#include <mln/core/range/range.hpp>

namespace mln
{

  template <typename I, typename SiteSet, typename STLOutputIterator>
  inline void wrt_offset(const Image<I>& ima_, const SiteSet& dpoints, STLOutputIterator out)
  {
    const I& ima = exact(ima_);

    std::ptrdiff_t strides[I::ndim];
    for (int i = 0; i < I::ndim; ++i)
      strides[i] = ima.byte_stride(i);

    auto it = rng::iter(dpoints);
    for (it.init(); !it.finished(); it.next(), ++out)
    {
      *out = 0;
      for (int i = 0; i < I::ndim; ++i)
        *out += strides[i] * (*it)[i];
    }
  }

  template <typename I, size_t N, typename P>
  inline void wrt_offset(const Image<I>& ima_,
                         const std::array<P, N>& dpoints,
                         std::array<typename I::difference_type, N>& out)
  {
    const I& ima = exact(ima_);

    std::ptrdiff_t strides[I::ndim];
    for (int i = 0; i < I::ndim; ++i)
      strides[i] = ima.byte_stride(i);

    for (size_t j = 0; j < N; ++j)
    {
      out[j] = 0;
      for (size_t i = 0; i < I::ndim; ++i)
        out[j] += static_cast<typename I::difference_type>(strides[i] * dpoints[j][i]);
    }
  }

  template <typename I, size_t N, typename P>
  inline void wrt_delta_index(const Image<I>& ima_, const std::array<P, N>& dpoints,
                              std::array<typename I::difference_type, N>& out)
  {
    const I& ima = exact(ima_);
    for (unsigned j = 0; j < N; ++j)
      out[j] = ima.delta_index(dpoints[j]);
  }

  template <typename I, typename SiteSet, typename OutputIterator>
  inline void wrt_delta_index(const Image<I>& ima_, const SiteSet& dpoints, OutputIterator out)
  {
    const I& ima = exact(ima_);
    auto it = rng::iter(dpoints);
    for (it.init(); !it.finished(); it.next(), ++out)
      *out = ima.delta_index(*it);
  }

  template <typename Image, size_t N, typename P>
  std::array<typename Image::index_type, N>
  wrt_offset(const Image& ima,
             const std::array<P, N>& dpoints)
  {
    std::array<typename Image::index_type, N> out;
    wrt_offset(ima, dpoints, out.begin());
    return out;
  }

  template <typename Image, size_t N, typename P>
  std::array<typename Image::index_type, N>
  wrt_delta_index(const Image& ima, const std::array<P, N>& dpoints)
  {
    std::array<typename Image::index_type, N> out;
    wrt_delta_index(ima, dpoints, out.begin());
    return out;
  }

} // end of namespace mln

#endif //! MLN_CORE_WRT_OFFSET_HPP

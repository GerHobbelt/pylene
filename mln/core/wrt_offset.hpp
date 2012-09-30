#ifndef MLN_CORE_WRT_OFFSET_HPP
# define MLN_CORE_WRT_OFFSET_HPP

//# include <mln/core/range.hpp>

namespace mln {

  template <typename Image, typename SiteSet, typename OutputIterator>
  inline
  void wrt_offset(const Image& ima, const SiteSet& dpoints, OutputIterator out)
  {
    const size_t* strides = ima.strides();
    typedef typename SiteSet::const_iterator Iterator;
    auto it = dpoints.iter();
    for (it.init(); !it.finished(); it.next(), ++out)
      {
        *out = 0;
        for (int i = 0; i < Image::ndim; ++i)
          *out += strides[i] * (*it)[i];
      }
  }

  template <typename Image, size_t N>
  inline
  void wrt_offset(const Image& ima,
                  const mln::array<typename Image::point_type, N>& dpoints,
                  mln::array<typename Image::difference_type, N>& out)
  {

    const size_t* strides = ima.strides();
    for (unsigned j = 0; j < N; ++j)
      {
        out[j] = 0;
        for (int i = 0; i < Image::ndim; ++i)
          out[j] += strides[i] * dpoints[j][i];
      }
  }


  template <typename Image, size_t N>
  mln::array<typename Image::difference_type, N>
  wrt_offset(const Image& ima, const mln::array<typename Image::point_type, N>& dpoints)
  {
    mln::array<typename Image::difference_type, N> out;
    wrt_offset(ima, dpoints, out);
    return out;
  }

} // end of namespace mln


#endif //!MLN_CORE_WRT_OFFSET_HPP

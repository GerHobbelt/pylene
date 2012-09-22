#ifndef MLN_CORE_WRT_OFFSET_HPP
# define MLN_CORE_WRT_OFFSET_HPP

# include <boost/range.hpp>

namespace mln {

  template <typename Image, typename SiteSet, typename OutputIterator>
  inline
  void wrt_offset(const Image& ima, const SiteSet& dpoints, OutputIterator out)
  {

    const size_t* strides = ima.strides();
    typedef typename boost::range_iterator<const SiteSet>::type Iterator;
    Iterator it = std::begin(dpoints);
    Iterator end = std::end(dpoints);
    for (;it != end; ++it, ++out)
      {
        *out = 0;
        for (int i = 0; i < Image::ndim; ++i)
          *out += strides[i] * (*it)[i];
      }
  }

  template <typename Image, size_t N, typename OutputIterator>
  inline
  void wrt_offset(const Image& ima,
                  const std::array<typename Image::point_type, N>& dpoints,
                  OutputIterator out)
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
  std::array<typename Image::difference_type, N>
  wrt_offset(const Image& ima, const std::array<typename Image::point_type, N>& dpoints)
    __attribute__ ((pure));

  template <typename Image, size_t N>
  std::array<typename Image::difference_type, N>
  wrt_offset(const Image& ima, const std::array<typename Image::point_type, N>& dpoints)
  {
    std::array<typename Image::difference_type, N> out;
    // const size_t* strides = ima.strides();
    // for (int j = 0; j < N; ++j)
    //   {
    //     out[j] = 0;
    //     for (int i = 0; i < Image::ndim; ++i)
    //       out[j] += strides[i] * dpoints[j][i];
    //   }


    wrt_offset(ima, dpoints, out.begin());
    return out;
  }

} // end of namespace mln


#endif //!MLN_CORE_WRT_OFFSET_HPP

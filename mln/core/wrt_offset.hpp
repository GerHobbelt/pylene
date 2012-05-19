#ifndef MLN_CORE_WRT_OFFSET_HPP
# define MLN_CORE_WRT_OFFSET_HPP

# include <boost/range.hpp>

namespace mln {

  template <typename Image, typename SiteSet, typename OutputIterator>
  void wrt_offset(const Image& ima, const SiteSet& dpoints, OutputIterator out)
  {

    const size_t* strides = ima.strides();
    typedef typename boost::range_iterator<const SiteSet>::type Iterator;
    Iterator it = boost::begin(dpoints);
    Iterator end = boost::end(dpoints);
    for (;it != end; ++it, ++out)
      {
        *out = 0;
        for (int i = 0; i < Image::ndim; ++i)
          *out += strides[i] * (*it)[i];
      }
  }

} // end of namespace mln


#endif //!MLN_CORE_WRT_OFFSET_HPP

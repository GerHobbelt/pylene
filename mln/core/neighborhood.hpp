#ifndef MLN_CORE_NEIGHBORHOOD_HPP
# define MLN_CORE_NEIGHBORHOOD_HPP

# include <type_traits>
# include <mln/core/neighborhood_pixel_range.hpp>
//# include <mln/core/pix_range_iterator_proxy.hpp>
# include <mln/core/iterator/sliding_win_piter.hpp>
# include <mln/core/range/iterator_range.hpp>

namespace mln {

  template <typename Derived, typename SiteSet>
  struct neighborhood_base
  {
    typedef typename SiteSet::value_type point_type;


    template <typename Pixel>
    using nbh = neighborhood_pixel_range<Pixel, SiteSet>;


    inline
    constexpr
    iterator_range< sliding_win_piter<SiteSet> >
    operator() (const point_type& p) const
    {
      return make_iterator_range( sliding_win_piter<SiteSet>(Derived::dpoints, p) );
    }


    template <typename PixelOrPixelIterator>
    inline
    nbh<PixelOrPixelIterator>
    operator() (const PixelOrPixelIterator& pix,
		typename std::enable_if<not std::is_convertible<PixelOrPixelIterator, point_type>::value>::type* = NULL) const
    {
      typedef nbh<PixelOrPixelIterator> nbh_t;

      nbh_t x(Derived::dpoints, pix);
      return x;
    }

    template <typename PixelOrPixelIterator,
	      typename = typename std::enable_if<not std::is_lvalue_reference<PixelOrPixelIterator>::value>::type>
    nbh<PixelOrPixelIterator>
    operator() (PixelOrPixelIterator&& pix) const
    {
      static_assert(not std::is_lvalue_reference<PixelOrPixelIterator>::value,
		    "You must pass a lvalue to neighborhood.");
    }

  };

} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_HPP

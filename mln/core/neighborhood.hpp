#ifndef MLN_CORE_NEIGHBORHOOD_HPP
# define MLN_CORE_NEIGHBORHOOD_HPP

# include <type_traits>
# include <mln/core/domain/dtranslate.hpp>
# include <mln/core/neighborhood_pixel_range.hpp>
# include <mln/core/pix_range_iterator_proxy.hpp>


namespace mln {

  template <typename Derived, typename SiteSet>
  struct neighborhood_base
  {
    typedef typename boost::range_value<SiteSet>::type point_type;


    template <typename Pixel>
    using nbh = neighborhood_pixel_range<Pixel, SiteSet>;


    inline
    constexpr
    translated_domain<SiteSet>
    operator() (point_type p) const
    {
      return dtranslate(Derived::dpoints, p);
    }



    template <typename Pixel>
    inline
    nbh<Pixel>
    operator() (const Pixel& pix) const;


    template <typename PixelRange>
    inline
    nbh< typename boost::range_value<PixelRange>::type >
    operator() (const internal::pix_range_iterator_proxy<PixelRange>& pix) const
    {
      typedef nbh< typename boost::range_value<PixelRange>::type > nbh_t;
      nbh_t x(Derived::dpoints, pix);
      return x;
    }

    template <typename Image>
    inline
    constexpr
    typename nbh<Image>::type
    operator() (const Image& ima, point_type p) const
    {
      static_assert(std::is_convertible<point_type, typename Image::point_type>::value, "Point is not convertible to Image::point_type");

      typename nbh<Image>::type x(ima, Derived::dpoints);
      x.center(p);
      return x;
    }
  };



    template <typename Derived, typename SiteSet>
    template <typename Pixel>
    typename neighborhood_base<Derived, SiteSet>::template nbh<Pixel>
    neighborhood_base<Derived, SiteSet>::operator() (const Pixel& pix) const
    {
      typedef nbh<Pixel> nbh_t;

      nbh_t x(Derived::dpoints, pix);
      return x;
    }


} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_HPP

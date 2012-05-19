#ifndef MLN_CORE_NEIGHBORHOOD_HPP
# define MLN_CORE_NEIGHBORHOOD_HPP

# include <boost/static_assert.hpp>
# include <boost/type_traits.hpp>
# include <mln/core/domain/dtranslate.hpp>

//# include <mln/core/sliding_image.hpp>

namespace mln {

  template <typename Derived, typename SiteSet>
  struct neighborhood_base
  {
    typedef typename boost::range_value<SiteSet>::type point_type;

    template <typename Image>
    struct nbh
    {
      typedef sliding_image<Image, SiteSet> type;
    };


    inline
    typename translated_domain<SiteSet>::type
    operator() (point_type p)
    {
      return dtranslate(Derived::dpoints, p);
    }

    template <typename Pixel>
    inline
    typename nbh<typename Pixel::image_type>::type
    operator() (Pixel pix)
    {
      typename nbh<typename Pixel::image_type>::type x(pix.image(), Derived::dpoints);
      x.center(&pix.val());
      return x;
    }


    template <typename Image>
    typename nbh<Image>::type
    operator() (const Image& ima, Point p)
    {
      BOOST_STATIC_ASSERT((boost::is_same<typename Image::point_type, Point>::value));

      typename nbh<Image>::type x(ima, Derived::dpoints);
      x.center(p);
      return x;
    }
  };

} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_HPP

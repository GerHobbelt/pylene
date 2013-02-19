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



    constexpr
    iterator_range< sliding_win_piter<SiteSet> >
    make_nbh(const point_type& p, std::true_type) const
    {
      return make_iterator_range( sliding_win_piter<SiteSet>(derived()->dpoints, p) );
    }


    template <typename PixelOrPixelIterator>
    nbh<PixelOrPixelIterator>
    make_nbh(const PixelOrPixelIterator& pix, std::false_type) const
    {
      typedef nbh<PixelOrPixelIterator> nbh_t;
      nbh_t x(derived()->dpoints, pix);
      return x;
    }


    template <typename PointOrPixelOrPixelIterator>
    auto operator() (PointOrPixelOrPixelIterator&& p) const
      -> decltype(this->make_nbh(std::forward<PointOrPixelOrPixelIterator>(p), std::is_convertible<PointOrPixelOrPixelIterator, point_type> ()))
    {
      static_assert(std::is_lvalue_reference<PointOrPixelOrPixelIterator>::value, "You must pass a lvalue to neighborhood.");
      return make_nbh(std::forward<PointOrPixelOrPixelIterator>(p), std::is_convertible<PointOrPixelOrPixelIterator, point_type> ());
    }


  private:
    const Derived* derived() const { return reinterpret_cast<const Derived*>(this); }

  };

} // end of namespace mln


#endif //!MLN_CORE_NEIGHBORHOOD_HPP

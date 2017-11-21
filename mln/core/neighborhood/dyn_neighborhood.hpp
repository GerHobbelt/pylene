#ifndef MLN_CORE_NEIGHBORHOOD_DYN_NEIGHBORHOOD_HPP
#define MLN_CORE_NEIGHBORHOOD_DYN_NEIGHBORHOOD_HPP

#include <mln/core/neighborhood/neighborhood_base.hpp>
#include <mln/core/neighborhood/sliding_piter.hpp>
#include <mln/core/neighborhood/sliding_pixter.hpp>
#include <mln/core/range/iterator_range.hpp>
#include <mln/core/range/range.hpp>
#include <mln/core/utils/wrapper.hpp>

namespace mln
{

  template <class SiteSet, class category, class E>
  struct dyn_neighborhood_base;

  template <class SiteSet, class category>
  struct dyn_neighborhood;

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class SiteSet, class category, class E>
  struct dyn_neighborhood_base : neighborhood_base<E, category>
  {
  public:
    typedef typename range_value<SiteSet>::type point_type;
    typedef typename range_value<SiteSet>::type site_type;

    auto __process_point(const point_type& p) const
    {
      return make_sliding_piter(make_value_wrapper(p), mln::exact(this)->dpoints);
    }

    template <typename P>
    auto __bind_point(P& p) const
    {
      return make_sliding_piter(std::cref(p), mln::exact(this)->dpoints);
    }

    template <typename PointIterator>
    auto __bind_point_iterator(const PointIterator& p) const
    {
      return make_sliding_piter(make_iterator_proxy(p), mln::exact(this)->dpoints);
    }

    template <typename Px>
    auto __bind_pixel(Px& px) const
    {
      return make_sliding_pixter(std::cref(px), mln::exact(this)->dpoints);
    }

    template <typename Px>
    auto __bind_pixel_iterator(const Px& px) const
    {
      return make_sliding_pixter(make_iterator_proxy(px), mln::exact(this)->dpoints);
    }
  };

  template <class SiteSet, class category>
  struct dyn_neighborhood : dyn_neighborhood_base<SiteSet, category, dyn_neighborhood<SiteSet, category>>
  {
    dyn_neighborhood(const SiteSet& pset) : dpoints{pset} {}

    const SiteSet dpoints;
  };
}

#endif // ! MLN_CORE_NEIGHBORHOOD_DYN_NEIGHBORHOOD_HPP

#ifndef MLN_CORE_NEIGHBORHOOD_DYN_WNEIGHBORHOOD_HPP
# define MLN_CORE_NEIGHBORHOOD_DYN_WNEIGHBORHOOD_HPP

# include <mln/core/neighborhood/neighborhood_base.hpp>
# include <mln/core/range/range.hpp>
# include <mln/core/range/iterator_range.hpp>
# include <mln/core/neighborhood/sliding_wpixter.hpp>
# include <mln/core/neighborhood/sliding_wpiter.hpp>
# include <mln/core/utils/iterator_proxy.hpp>

namespace mln
{

  template <class SiteSet, class WeightSet, class category, class E>
  struct dyn_wneighborhood_base;

  template <class SiteSet, class WeightSet, class category>
  struct dyn_wneighborhood;


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class SiteSet, class WeightSet, class category, class E>
  struct dyn_wneighborhood_base : neighborhood_base<E, category>
  {
  public:
    typedef typename range_value<SiteSet>::type point_type;
    typedef typename range_value<SiteSet>::type site_type;

    auto
    __process_point(const point_type& p) const
    {
      return make_iterator_range( make_sliding_wpiter(p, exact(this)->dpoints, exact(this)->weights) );
    }

    template <typename P>
    auto
    __bind_point(P& p) const
    {
      return make_iterator_range( make_sliding_wpiter(std::cref(p), exact(this)->dpoints, exact(this)->weights) );
    }

    template <typename PointIterator>
    auto
    __bind_point_iterator(const PointIterator& p) const
    {
      return make_iterator_range( make_sliding_wpiter(make_iterator_proxy(p), exact(this)->dpoints, exact(this)->weights) );
    }

    template <typename Px>
    auto
    __bind_pixel(Px& px) const
    {
      return make_iterator_range( make_sliding_wpixter(std::cref(px), exact(this)->dpoints, exact(this)->weights) );
    }

    template <typename Px>
    auto
    __bind_pixel_iterator(const Px& px) const
    {
      return make_iterator_range( make_sliding_wpixter(make_iterator_proxy(px), exact(this)->dpoints, exact(this)->weights) );
    }

  };

  template <class SiteSet, class WeightSet, class category>
  struct dyn_wneighborhood : dyn_wneighborhood_base<SiteSet, WeightSet, category,
                                                    dyn_wneighborhood<SiteSet, WeightSet, category> >
  {
    dyn_wneighborhood(const SiteSet& pset, const WeightSet& wset)
      : dpoints{pset}, weights{wset}
    {
    }

    const SiteSet   dpoints;
    const WeightSet weights;
  };
}

#endif // ! MLN_CORE_NEIGHBORHOOD_DYN_WNEIGHBORHOOD_HPP

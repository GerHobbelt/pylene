#pragma once

#include <mln/core/neighborhood/neighborhood_base.hpp>
#include <mln/core/neighborhood/sliding_piter.hpp>
#include <mln/core/neighborhood/sliding_pixter.hpp>
#include <mln/core/range/iterator_range.hpp>
#include <mln/core/range/range.hpp>
#include <mln/core/utils/wrapper.hpp>


namespace mln
{

  template <class category, class E>
  struct dyn_neighborhood_base;

  template <class SiteSet, class category>
  struct dyn_neighborhood;

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class category, class E>
  struct dyn_neighborhood_base : neighborhood_base<E, category>
  {
  public:
    template <typename P>
    auto __process_point(const P& p) const
    {
      return make_sliding_piter(make_value_wrapper(p), mln::exact(this)->offsets());
    }

    template <typename P>
    auto __bind_point(P& p) const
    {
      return make_sliding_piter(std::cref(p), mln::exact(this)->offsets());
    }

    template <typename PointIterator>
    auto __bind_point_iterator(const PointIterator& p) const
    {
      return make_sliding_piter(make_iterator_proxy(p), mln::exact(this)->offsets());
    }

    template <typename Px>
    auto __bind_pixel(Px& px) const
    {
      return make_sliding_pixter(std::cref(px), mln::exact(this)->offsets());
    }

    template <typename Px>
    auto __bind_pixel_iterator(const Px& px) const
    {
      return make_sliding_pixter(make_iterator_proxy(px), mln::exact(this)->offsets());
    }
  };

  template <class SiteSet, class category>
  struct dyn_neighborhood : dyn_neighborhood_base<category, dyn_neighborhood<SiteSet, category>>
  {
    dyn_neighborhood(const SiteSet& pset)
      : dpoints{pset}
    {
    }
    dyn_neighborhood(SiteSet&& pset)
      : dpoints(std::move(pset))
    {
    }

    const SiteSet& offsets() const { return dpoints; }

  private:
    const SiteSet dpoints;
  };
} // namespace mln
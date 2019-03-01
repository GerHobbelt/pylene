#pragma once

#include <mln/core/neighborhood/dyn_neighborhood.hpp>

// Fwd declaration


namespace mln
{

  namespace se
  {

    template <class SiteSet>
    dyn_neighborhood<SiteSet, dynamic_neighborhood_tag> make_se(const SiteSet& s);

    template <class Point, unsigned N>
    dyn_neighborhood<std::array<Point, N>, constant_neighborhood_tag> make_se(const std::array<Point, N>& s);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class SiteSet>
    dyn_neighborhood<SiteSet, dynamic_neighborhood_tag> make_se(const SiteSet& s)
    {
      return {s};
    }

    template <class Point, unsigned N>
    dyn_neighborhood<std::array<Point, N>, constant_neighborhood_tag> make_se(const std::array<Point, N>& s)
    {
      return {s};
    }

  } // namespace se

} // namespace mln

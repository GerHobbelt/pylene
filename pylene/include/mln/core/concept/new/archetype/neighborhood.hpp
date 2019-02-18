#pragma once

#include <mln/core/concept/new/neighborhood.hpp>
#include <mln/core/concept/new/archetype/se.hpp>


namespace mln::archetypes
{

  namespace details
  {
    template <class P, class Pix>
#ifdef PYLENE_CONCEPT_TS_ENABLED
    requires mln::concepts::Point<P> && mln::concepts::Pixel<Pix>
#endif
    struct Neighborhood : StructuringElement<P, Pix>
    {
      ::ranges::iterator_range<P*>   before(P p);
      ::ranges::iterator_range<P*>   after(P p);
      ::ranges::iterator_range<Pix*> before(Pix px);
      ::ranges::iterator_range<Pix*> after(Pix px);
    };


    template <class N>
    struct AsNeighborhood : N, mln::experimental::Neighborhood<AsNeighborhood<N>>
    {
    };

  } // namespace details

  template <class P = Point, class Pix = PixelT<P>>
  using Neighborhood = details::AsSE<details::Neighborhood<P, Pix>>;

} // namespace mln::archetypes


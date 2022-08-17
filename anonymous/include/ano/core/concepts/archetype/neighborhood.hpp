#pragma once

#include <ano/core/concepts/archetype/structuring_element.hpp>
#include <ano/core/concepts/neighborhood.hpp>


namespace ano::archetypes
{

  namespace details
  {
    template <class P, class Pix>
    // clang-format off
    requires ano::concepts::Point<P>&& ano::concepts::Pixel<Pix>
    struct Neighborhood : StructuringElement<P, Pix>
    // clang-format on
    {
      ::ranges::iterator_range<P*>   before(P p);
      ::ranges::iterator_range<P*>   after(P p);
      ::ranges::iterator_range<Pix*> before(Pix px);
      ::ranges::iterator_range<Pix*> after(Pix px);
    };


    template <class N>
    struct AsNeighborhood : N, ano::details::Neighborhood<AsNeighborhood<N>>
    {
    };

  } // namespace details

  template <class P = Point, class Pix = PixelT<P>>
  using Neighborhood = details::AsSE<details::Neighborhood<P, Pix>>;

} // namespace ano::archetypes

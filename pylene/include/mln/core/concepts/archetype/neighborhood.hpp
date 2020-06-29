#pragma once

#include <mln/core/concepts/archetype/structuring_element.hpp>
#include <mln/core/concepts/neighborhood.hpp>


namespace mln::archetypes
{

  namespace details
  {
    template <class P, class Pix>
    // clang-format off
    requires mln::concepts::Point<P>&& mln::concepts::Pixel<Pix>
#endif
    struct Neighborhood : StructuringElement<P, Pix>
    // clang-format on
    {
      ::ranges::iterator_range<P*>   before(P p);
      ::ranges::iterator_range<P*>   after(P p);
      ::ranges::iterator_range<Pix*> before(Pix px);
      ::ranges::iterator_range<Pix*> after(Pix px);
    };


    template <class N>
    struct AsNeighborhood : N, mln::details::Neighborhood<AsNeighborhood<N>>
    {
    };

  } // namespace details

  template <class P = Point, class Pix = PixelT<P>>
  using Neighborhood = details::AsSE<details::Neighborhood<P, Pix>>;

} // namespace mln::archetypes

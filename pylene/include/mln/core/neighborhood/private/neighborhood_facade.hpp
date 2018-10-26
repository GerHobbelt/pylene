#pragma once

#include <mln/core/concept/neighborhood.hpp>
#include <mln/core/concept/pixel.hpp>
#include <range/v3/view/transform.hpp>
#include <type_traits>

namespace mln
{

  template <class N>
  struct neighborhood_facade : Neighborhood<N>
  {
    template <class P, std::enable_if_t<is_a<P, Pixel>::value, long> = 0>
    auto operator()(const P& pixel) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->offsets(),
                                 [pixel](auto offset) -> P
                                 {
                                   P tmp = pixel;
                                   tmp.advance(offset);
                                   return tmp;
                                 });
    }

    template <class P, std::enable_if_t<!is_a<P, Pixel>::value, int> = 0>
    auto operator()(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->offsets(),
                                       [point](P offset) -> P { return point + offset; });
    }

  };

}

#pragma once

#include <mln/core/concept/new/neighborhoods.hpp>
#include <mln/core/neighborhood/private/sliding_pixel_range.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>

namespace mln
{

  template <class N>
  struct neighborhood_facade : experimental::Neighborhood<N>
  {
    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto operator()(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->offsets()};
    }

    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto before(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->before_offsets()};
    }

    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto after(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->after_offsets()};
    }


    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto operator()(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->offsets(),
                                       [point](P offset) -> P { return point + offset; });
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto before(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->before_offsets(),
                                       [point](P offset) -> P { return point + offset; });
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto after(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->after_offsets(),
                                       [point](P offset) -> P { return point + offset; });
    }
  };
} // namespace mln

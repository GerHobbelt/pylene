#pragma once

#include <mln/core/concept/object.hpp>
#include <mln/core/concept/neighborhoods.hpp>
#include <mln/core/concept/pixels.hpp>

#include <mln/core/neighborhood/private/sliding_pixel_range.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>

namespace mln
{
  namespace details
  {
    template <class P>
    struct add_point
    {
      P operator() (P offset) const noexcept { return x + offset; }

      P x;
    };

    template <class P>
    struct add_wpoint
    {
      template <class Q>
      Q operator()(Q offset) const noexcept
      {
        offset += x;
        return offset;
      }

      P x;
    };
  }

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
      return ::ranges::view::transform(static_cast<const N*>(this)->offsets(), details::add_point<P>{point});
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto before(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->before_offsets(), details::add_point<P>{point});
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto after(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->after_offsets(), details::add_point<P>{point});
    }
  };

  template <class N>
  struct wneighborhood_facade : experimental::Neighborhood<N>
  {
    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto operator()(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->offsets()};
    }

    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto before(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->before_offsets()};
    }

    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto after(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->after_offsets()};
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto operator()(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->offsets(), details::add_wpoint<P>{point});
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto before(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->before_offsets(), details::add_wpoint<P>{point});
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto after(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const N*>(this)->after_offsets(), details::add_wpoint<P>{point});
    }
  };
} // namespace mln

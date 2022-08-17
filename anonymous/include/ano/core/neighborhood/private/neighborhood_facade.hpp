#pragma once

#include <ano/core/concepts/neighborhood.hpp>
#include <ano/core/concepts/object.hpp>
#include <ano/core/concepts/pixel.hpp>

#include <ano/core/neighborhood/private/sliding_pixel_range.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>

namespace ano
{
  namespace details
  {
    template <class P>
    struct add_point
    {
      P operator()(P offset) const noexcept { return x + offset; }

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
  } // namespace details

  template <class N>
  struct neighborhood_facade : ano::details::Neighborhood<N>
  {
    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->offsets()};
    }

    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto before(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->before_offsets()};
    }

    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto after(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const N*>(this)->after_offsets()};
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->offsets(), details::add_point<P>{point});
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto before(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->before_offsets(), details::add_point<P>{point});
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto after(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->after_offsets(), details::add_point<P>{point});
    }
  };

  template <class N>
  struct wneighborhood_facade : ano::details::Neighborhood<N>
  {
    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->offsets()};
    }

    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto before(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->before_offsets()};
    }

    template <class P>
    requires(ano::is_a<P, ano::details::Pixel>::value) auto after(const P& pixel) const
    {
      return details::sliding_wpixel_range{pixel, static_cast<const N*>(this)->after_offsets()};
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->offsets(), details::add_wpoint<P>{point});
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto before(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->before_offsets(), details::add_wpoint<P>{point});
    }

    template <class P>
    requires(!ano::is_a<P, ano::details::Pixel>::value) auto after(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const N*>(this)->after_offsets(), details::add_wpoint<P>{point});
    }
  };
} // namespace ano

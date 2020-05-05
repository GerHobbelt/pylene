#pragma once

#include <mln/core/concepts/structuring_element.hpp>
#include <mln/core/concepts/pixel.hpp>
#include <mln/core/concepts/object.hpp>

#include <mln/core/neighborhood/private/sliding_pixel_range.hpp>
#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>


namespace mln
{
  template <class SE>
  struct se_facade : mln::details::StructuringElement<SE>
  {
    template <class P>
    requires(mln::is_a<P, mln::details::Pixel>::value) auto operator()(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const SE*>(this)->offsets()};
    }

    template <class P>
    requires(!mln::is_a<P, mln::details::Pixel>::value) auto operator()(const P& point) const
    {
      return ::ranges::views::transform(static_cast<const SE*>(this)->offsets(), details::add_point<P>{point});
    }
  };

} // namespace mln

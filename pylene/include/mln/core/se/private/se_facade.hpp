#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/neighborhood/private/sliding_pixel_range.hpp>
#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <range/v3/view/transform.hpp>

#include <type_traits>


namespace mln
{

  template <class SE>
  struct se_facade : experimental::StructuringElement<SE>
  {
    template <class P, std::enable_if_t<is_a<P, mln::experimental::Pixel>::value, long> = 0>
    auto operator()(const P& pixel) const
    {
      return details::sliding_pixel_range{pixel, static_cast<const SE*>(this)->offsets()};
    }

    template <class P, std::enable_if_t<!is_a<P, mln::experimental::Pixel>::value, int> = 0>
    auto operator()(const P& point) const
    {
      return ::ranges::view::transform(static_cast<const SE*>(this)->offsets(), details::add_point<P>{point});
    }
  };

} // namespace mln

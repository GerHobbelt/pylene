#pragma once

#include <mln/core/concepts/structuring_element.hpp>

#include <concepts/concepts.hpp>
#include <range/v3/range/concepts.hpp>

namespace mln::details
{
  template <class SE>
  struct Neighborhood : StructuringElement<SE>
  {
  };
} // namespace mln::details


namespace mln::concepts
{

  // clang-format off
  template <typename SE, typename P>
  concept Neighborhood =
    StructuringElement<SE, P> &&
    requires (SE se, P p, mln::archetypes::PixelT<P> px) {
      { se.before(p) }  -> ::ranges::cpp20::forward_range;
      { se.after(p) }   -> ::ranges::cpp20::forward_range;
      { se.before(px) } -> ::ranges::cpp20::forward_range;
      { se.after(px) }  -> ::ranges::cpp20::forward_range;

      requires ::concepts::convertible_to<::ranges::range_value_t<decltype(se.before(p))>, P>;
      requires ::concepts::convertible_to<::ranges::range_value_t<decltype(se.after(p))>, P>;
      requires concepts::Pixel<::ranges::range_value_t<decltype(se.before(px))>>;
      requires concepts::Pixel<::ranges::range_value_t<decltype(se.after(px))>>;
    };
  // clang-format on

} // namespace mln::concepts

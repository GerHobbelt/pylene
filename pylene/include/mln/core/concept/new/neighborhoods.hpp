#pragma once

#include <mln/core/concept/new/structuring_elements.hpp>

namespace mln::experimental
{

  template <class SE>
  struct Neighborhood : StructuringElement<SE>
  {
  };

} // namespace mln::experimental


namespace mln::concepts
{

  // clang-format off
#ifdef PYLENE_CONCEPT_TS_ENABLED
  template <typename SE, typename P>
  concept Neighborhood =
    StructuringElement<SE, P> &&
    requires (SE se, P p, mln::archetypes::PixelT<P> px) {
      { se.before(p) } -> stl::ForwardRange&&;
      { se.after(p) } -> stl::ForwardRange&&;
      { se.before(px) } -> stl::ForwardRange&&;
      { se.after(px) } -> stl::ForwardRange&&;

      requires detail::RangeValueTypeConvertibleTo<decltype(se.before(p)), P>;
      requires detail::RangeValueTypeConvertibleTo<decltype(se.after(p)), P>;
      requires concepts::Pixel<::ranges::range_value_t<decltype(se.before(px))>>;
      requires concepts::Pixel<::ranges::range_value_t<decltype(se.after(px))>>;
    };
#endif
  // clang-format on

} // namespace mln::concepts

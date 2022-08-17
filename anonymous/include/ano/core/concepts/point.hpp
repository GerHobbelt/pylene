#pragma once

#include <concepts/concepts.hpp>

namespace ano::concepts
{

  // clang-format off

  // Fixme: Point is allready a symbol in ano:: (conflicts with GCC-9)
  template <typename P>
  concept PointC =
    ::concepts::regular<P> &&
    ::concepts::totally_ordered<P>;
  // clang-format on

  template <typename P>
  inline constexpr bool Point = PointC<P>;

} // namespace ano::concepts

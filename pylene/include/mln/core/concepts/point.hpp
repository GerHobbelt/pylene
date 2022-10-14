#pragma once

#include <concepts>

namespace mln::concepts
{

  // clang-format off

  // Fixme: Point is allready a symbol in mln:: (conflicts with GCC-9)
  template <typename P>
  concept PointC =
    std::regular<P> &&
    std::totally_ordered<P>;
  // clang-format on

  template <typename P>
  inline constexpr bool Point = PointC<P>;

} // namespace mln::concepts


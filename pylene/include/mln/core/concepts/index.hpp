#pragma once

#include <concepts>

namespace mln::concepts
{

  // clang-format off
  template <typename Idx>
  concept Index = std::signed_integral<Idx>;

  // clang-format on

} // namespace mln::concepts

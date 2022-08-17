#pragma once

#include <concepts/concepts.hpp>

namespace ano::concepts
{

  // clang-format off
  template <typename Idx>
  concept Index = ::concepts::signed_integral<Idx>;

  // clang-format on

} // namespace ano::concepts

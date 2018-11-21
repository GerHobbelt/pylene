#pragma once

#include <mln/core/concept/new/stl/fundamentals.hpp>

namespace mln::concepts
{

  // clang-format off

  // Index
  template<typename Idx>
  concept bool Index = stl::Integral<Idx>;

  // clang-format on

} // namespace mln::concepts

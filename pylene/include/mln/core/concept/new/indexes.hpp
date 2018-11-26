#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef CONCEPT_TS_ENABLED
  // Index
  template<typename Idx>
  concept bool Index = stl::Integral<Idx>;
#endif

  // clang-format on

} // namespace mln::concepts

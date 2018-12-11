#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Index
  template<typename Idx>
  // FIXME: switch to SignedIntegral when concept is consolidated
  concept Index = /* stl::SignedIntegral<Idx>; */
    stl::Integral<Idx>;
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/index.hpp>

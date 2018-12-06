#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Index
  template<typename Idx>
  concept Index = stl::SignedIntegral<Idx>;
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

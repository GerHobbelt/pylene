#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <type_traits>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Domain
  template<typename Dom>
  concept Domain = 
    stl::ForwardRange<Dom> &&
    Point<typename Dom::value_type> &&
    requires(const Dom cdom, typename Dom::value_type p) {
      { cdom.has(p) }   -> bool;
      { cdom.empty() }  -> bool;
      { cdom.size() }   -> stl::UnsignedIntegral&&;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

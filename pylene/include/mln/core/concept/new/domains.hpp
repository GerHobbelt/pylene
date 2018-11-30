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
  concept bool Domain = 
    stl::ForwardRange<Dom> &&
    requires {
        typename Dom::point_type;
    } &&
    Point<typename Dom::point_type> &&
    !std::is_const_v<typename Dom::codomain_type> &&
    !std::is_reference_v<typename Dom::codomain_type> &&
    requires(const Dom cdom, const typename Dom::point_type& p) {
        { cdom.has(p) }             -> bool&&;
        { cdom.empty() }            -> bool&&;
        { cdom.continuous() }       -> bool&&;
        { cdom.discrete() }         -> bool&&;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

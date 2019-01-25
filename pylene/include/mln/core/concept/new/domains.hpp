#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <mln/core/domain/private/domain_traits.hpp>

#include <type_traits>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  // Domain
  template <typename Dom>
  concept Domain = 
    stl::ForwardRange<Dom> &&
    stl::Same<domain_value_t<Dom>, domain_point_t<Dom>> &&
    Point<domain_point_t<Dom>> &&
    requires(const Dom cdom, domain_point_t<Dom> p) {
      { cdom.has(p) }   -> bool;
      { cdom.empty() }  -> bool;
      // { cdom.size() }   -> stl::UnsignedIntegral&&;
    };


  // SizedDomain
  template <typename Dom>
  concept SizedDomain = 
    Domain<Dom> &&
    requires(const Dom cdom) {
      { cdom.size() } -> stl::UnsignedIntegral&&;
    };

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/domain.hpp>

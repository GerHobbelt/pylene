#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>
#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <range/v3/range_traits.hpp>

#include <array>
#include <type_traits>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  // Domain
  template <typename Dom>
  concept Domain =
    stl::ForwardRange<Dom> &&
    Point<::ranges::range_value_t<Dom>> &&
    requires(const Dom cdom, ::ranges::range_value_t<Dom> p) {
      { cdom.has(p) }   -> bool;
      { cdom.empty() }  -> bool;
      { cdom.dim() }    -> int;
    };


  // SizedDomain
  template <typename Dom>
  concept SizedDomain = 
    Domain<Dom> &&
    requires(const Dom cdom) {
      { cdom.size() } -> stl::UnsignedIntegral&&;
    };
  
  // ShapedDomain
  template <typename Dom>
  concept ShapedDomain = 
    SizedDomain<Dom> &&
    requires(const Dom cdom) {
      { cdom.shape() }    -> ::ranges::range_value_t<Dom>;
      { cdom.extents() }  -> stl::ForwardRange&&;
    };

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/domain.hpp>

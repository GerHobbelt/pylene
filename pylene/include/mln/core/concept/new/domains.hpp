#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <type_traits>

namespace mln::concepts
{

  // clang-format off

#ifdef CONCEPT_TS_ENABLED
  // Domain
  template<typename Dom>
  concept bool Domain = 
    stl::Regular<Dom> &&
    requires {
        typename Dom::point_type;
        typename Dom::codomain_type;
    } &&
    Point<typename Dom::point_type> &&
    Value<typename Dom::codomain_type> &&
    !std::is_const_v<typename Dom::codomain_type> &&
    !std::is_reference_v<typename Dom::codomain_type> &&
    requires(const Dom cdom, const typename Dom::point_type& p) {
        { cdom.has(p) }             -> stl::Boolean&&;
        { cdom.empty() }            -> stl::Boolean&&;
        { cdom.is_continuous() }    -> stl::Boolean&&;
        { cdom.is_discrete() }      -> stl::Boolean&&;
        { cdom.points() }           -> stl::BidirectionalRange&&;
    };
#endif

  // clang-format on

} // namespace mln::concepts

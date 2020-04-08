#pragma once

#include <mln/core/concept/points.hpp>
#include <mln/core/concept/values.hpp>


#include <mln/core/rangev3/concepts.hpp>
#include <mln/core/rangev3/type_traits.hpp>

#include <array>
#include <type_traits>


namespace mln::concepts
{

  // clang-format off


  // Domain
  template <typename Dom>
  concept Domain =
    mln::ranges::mdrange<Dom> &&
    Point<mln::ranges::mdrange_value_t<Dom>> &&
    requires(const Dom cdom, mln::ranges::mdrange_value_t<Dom> p) {
      { cdom.has(p) }   -> bool;
      { cdom.empty() }  -> bool;
      { cdom.dim() }    -> int;
    };


  // SizedDomain
  template <typename Dom>
  concept SizedDomain =
    Domain<Dom> &&
    requires(const Dom cdom) {
    { cdom.size() } -> ::concepts::unsigned_integral;
    };

  // ShapedDomain
  template <typename Dom>
  concept ShapedDomain =
    SizedDomain<Dom> &&
    requires(const Dom cdom) {
      { cdom.shape() }    -> ::ranges::range_value_t<Dom>;
      { cdom.extents() }  -> ::ranges::cpp20::forward_range;
    };


  // clang-format on

} // namespace mln::concepts

// Validate concept
//#include <mln/core/concept/archetype/domain.hpp>

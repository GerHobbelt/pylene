#pragma once

#include <ano/core/concepts/point.hpp>
#include <ano/core/concepts/range.hpp>
#include <ano/core/concepts/value.hpp>

#include <ano/core/range/type_traits.hpp>
#include <type_traits>


namespace ano::concepts
{

  // clang-format off


  // Domain
  template <typename Dom>
  concept Domain =
    ano::ranges::mdrange<Dom> &&
    Point<ano::ranges::mdrange_value_t<Dom>> &&
    requires(const Dom cdom, ano::ranges::mdrange_value_t<Dom> p) {
    { cdom.has(p) }   -> ::concepts::same_as<bool>;
    { cdom.empty() }  -> ::concepts::same_as<bool>;
    { cdom.dim() }    -> ::concepts::same_as<int>;
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
      { cdom.extents() }  -> ::ranges::cpp20::forward_range;
    };


  // clang-format on

} // namespace ano::concepts

// Validate concept
//#include <ano/core/concepts/archetype/domain.hpp>

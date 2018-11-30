#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/indexes.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <type_traits>
#include <utility>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Image
  template<typename Ima>
  concept bool Image = 
    stl::Regular<Ima> &&
    requires {
      typename Ima::pixel_type;
      typename Ima::point_type;
      typename Ima::value_type;
      typename Ima::domain_type;
      typename Ima::difference_type;
      typename Ima::index_type;
      typename Ima::reference;
    } &&
    Pixel<typename Ima::pixel_type> &&
    Point<typename Ima::point_type> &&
    Value<typename Ima::value_type> &&
    Domain<typename Ima::domain_type> &&
    Index<typename Ima::index_type> &&
    stl::SignedIntegral<typename Ima::difference_type> &&
    stl::Same<typename Ima::difference_type, decltype(std::declval<typename Ima::index_type>() - std::declval<typename Ima::index_type>())> &&
    stl::ConvertibleTo<typename Ima::point_type, typename Ima::pixel_type::point_type> &&
    stl::ConvertibleTo<typename Ima::value_type, typename Ima::pixel_type::value_type> &&
    stl::ConvertibleTo<typename Ima::reference, typename Ima::pixel_type::reference> &&
    !std::is_const_v<typename Ima::value_type> &&
    !std::is_reference_v<typename Ima::value_type> &&
    requires(   const Ima cima, typename Ima::domain_type d,
                typename Ima::index_type k, typename Ima::point_type p) {
        { cima[k] }                 -> typename Ima::reference&&;
        { cima.pixels() }           -> stl::ForwardRange&&;
        { cima.values() }           -> stl::ForwardRange&&;
        { cima.domain() }           -> typename Ima::domain_type&&;
        { cima.point_of_index(k) }  -> typename Ima::point_type&&;
        { cima.index_at_point(p) }  -> typename Ima::index_type&&;
        { cima.delta_index(p) }     -> typename Ima::difference_type&&;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

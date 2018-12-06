#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/values.hpp>

#include <type_traits>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Pixel
  template<typename Pix>
  concept Pixel = 
    stl::Regular<Pix> &&
    requires {
      typename Pix::value_type;
      typename Pix::point_type;
      typename Pix::reference;
    } &&
    Value<typename Pix::value_type> &&
    Point<typename Pix::point_type> &&
    !std::is_const_v<typename Pix::value_type> &&
    !std::is_reference_v<typename Pix::value_type> &&
    requires(const Pix cpix) {
      { cpix.point() } -> stl::Same<typename Pix::point_type>&&;
      { cpix.val() } -> stl::Same<typename Pix::reference>&&;
    };

    // TODO writable pixel + output/input pixel aliases
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/pixel.hpp>

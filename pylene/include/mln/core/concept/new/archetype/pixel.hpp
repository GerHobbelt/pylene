#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/pixels.hpp>


namespace mln::concepts::archetype
{

  struct pixel_archetype final
  {
    using value_type = value_archetype;
    using point_type = point_archetype;
    using reference  = value_archetype&;

    point_type point() const;
    reference  val() const;
  };

  bool operator==(const pixel_archetype& lhs, const pixel_archetype& rhs);
  bool operator!=(const pixel_archetype& lhs, const pixel_archetype& rhs);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Pixel<pixel_archetype>, "pixel_archetype does not model the Pixel concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype

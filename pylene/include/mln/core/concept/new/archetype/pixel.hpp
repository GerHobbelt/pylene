#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/pixels.hpp>


namespace mln::archetypes
{

  struct Pixel
  {
    using value_type = Value;
    using point_type = Point;
    using reference  = Value&;

    point_type point() const;
    reference  val() const;
  };

  bool operator==(const Pixel& lhs, const Pixel& rhs);
  bool operator!=(const Pixel& lhs, const Pixel& rhs);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Pixel<Pixel>, "Pixel archetype does not model the Pixel concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputPixel final : Pixel
  {
    using Pixel::val;
    reference val();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputPixel<OutputPixel>,
                "OutputPixel archetype does not model the OutputPixel concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes

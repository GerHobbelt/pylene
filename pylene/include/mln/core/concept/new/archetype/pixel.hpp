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
    using reference  = const value_type&;

    Pixel()             = delete;
    Pixel(const Pixel&) = default;
    Pixel(Pixel&&)      = default;
    Pixel& operator=(const Pixel&) = default;
    Pixel& operator=(Pixel&&) = default;

    point_type point() const;
    reference  val() const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Pixel<Pixel>, "Pixel archetype does not model the Pixel concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputPixel final : Pixel
  {
    using reference = value_type&;

    reference val() const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputPixel<OutputPixel>,
                "OutputPixel archetype does not model the OutputPixel concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes

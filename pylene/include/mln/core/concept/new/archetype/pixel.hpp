#pragma once

#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/check.hpp>
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
    Pixel& operator=(const Pixel&) = delete;
    Pixel& operator=(Pixel&&) = delete;

    point_type point() const;
    reference  val() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Pixel<Pixel>, "Pixel archetype does not model the Pixel concept!");


  struct OutputPixel final : Pixel
  {
    using reference = value_type&;

    reference val() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::OutputPixel<OutputPixel>,
                           "OutputPixel archetype does not model the OutputPixel concept!");

} // namespace mln::archetypes

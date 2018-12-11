#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>

#include <tuple>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;


struct A
{
};

TEST(Core, Concept_Pixel)
{
  static_assert(!concepts::Pixel<int>);
  static_assert(!concepts::Pixel<A>);
  static_assert(concepts::Pixel<archetypes::Pixel>);
}

TEST(Core, Concept_OutputPixel)
{
  static_assert(!concepts::OutputPixel<int>);
  static_assert(!concepts::OutputPixel<A>);
  static_assert(!concepts::OutputPixel<archetypes::Pixel>);
  static_assert(concepts::OutputPixel<archetypes::OutputPixel>);
}

#include <mln/core/concept/archetype/pixel.hpp>
#include <mln/core/concept/pixels.hpp>

#include <tuple>

#include <gtest/gtest.h>


struct A
{
};

TEST(Core, Concept_Pixel)
{
  static_assert(not mln::concepts::Pixel<int>);
  static_assert(not mln::concepts::Pixel<A>);
  static_assert(mln::concepts::Pixel<mln::archetypes::Pixel>);
}

TEST(Core, Concept_OutputPixel)
{
  static_assert(not mln::concepts::OutputPixel<int>);
  static_assert(not mln::concepts::OutputPixel<A>);
  static_assert(not mln::concepts::OutputPixel<mln::archetypes::Pixel>);
  static_assert(mln::concepts::OutputPixel<mln::archetypes::OutputPixel>);
}

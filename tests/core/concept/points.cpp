#include <mln/core/concepts/archetype/point.hpp>
#include <mln/core/concepts/point.hpp>

#include <gtest/gtest.h>


struct A
{
};

TEST(Core, Concept_Point)
{
  static_assert(mln::concepts::Point<int>);
  static_assert(mln::concepts::Point<double>);
  static_assert(not mln::concepts::Point<A>);
  static_assert(mln::concepts::Point<mln::archetypes::Point>);
}

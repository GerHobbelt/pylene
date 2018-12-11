#include <mln/core/concept/new/archetype/point.hpp>
#include <mln/core/concept/new/points.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

struct A
{
};

TEST(Core, Concept_Point)
{
  static_assert(concepts::Point<int>);
  static_assert(concepts::Point<double>);
  static_assert(!concepts::Point<A>);
  static_assert(concepts::Point<archetypes::Point>);
}

#include <mln/core/concept/archetype/neighborhood.hpp>
#include <mln/core/concept/neighborhoods.hpp>

#include <gtest/gtest.h>

TEST(Core, Concept_Neighborhood)
{
  static_assert(mln::concepts::Neighborhood<mln::archetypes::Neighborhood<>, mln::archetypes::Point>);
}

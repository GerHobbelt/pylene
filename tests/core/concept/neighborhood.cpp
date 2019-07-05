#include <mln/core/concept/new/archetype/neighborhood.hpp>
#include <mln/core/concept/new/neighborhoods.hpp>

#include <gtest/gtest.h>

TEST(Core, Concept_Neighborhood)
{
  static_assert(mln::concepts::Neighborhood<mln::archetypes::Neighborhood<>, mln::archetypes::Point>);
}

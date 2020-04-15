#include <mln/core/concepts/archetype/neighborhood.hpp>
#include <mln/core/concepts/neighborhood.hpp>

#include <gtest/gtest.h>

TEST(Core, Concept_Neighborhood)
{
  static_assert(mln::concepts::Neighborhood<mln::archetypes::Neighborhood<>, mln::archetypes::Point>);
}

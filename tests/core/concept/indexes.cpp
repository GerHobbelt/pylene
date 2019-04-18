#include <mln/core/concept/new/archetype/index.hpp>
#include <mln/core/concept/new/indexes.hpp>

#include <gtest/gtest.h>


struct A
{
};
enum B : int
{
  bval
};

TEST(Core, Concept_Index)
{
  // TODO: test signed/unsigned once  concept is fixed and consolidated
  static_assert(mln::concepts::Index<int>);
  static_assert(not mln::concepts::Index<double>);
  static_assert(not mln::concepts::Index<A>);
  static_assert(not mln::concepts::Index<B>);
  static_assert(mln::concepts::Index<mln::archetypes::Index>);
}

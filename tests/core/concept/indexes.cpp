#include <mln/core/concept/new/archetype/index.hpp>
#include <mln/core/concept/new/indexes.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

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
  static_assert(concepts::Index<int>);
  static_assert(!concepts::Index<double>);
  static_assert(!concepts::Index<A>);
  static_assert(!concepts::Index<B>);
  static_assert(concepts::Index<archetypes::Index>);
}

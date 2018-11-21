#include <mln/core/concept/new/indexes.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

struct A
{
};
enum B : int
{
  bval
};

TEST(Core, Concept_Index)
{
  static_assert(concepts::Index<int>);
  static_assert(!concepts::Index<double>);
  static_assert(!concepts::Index<A>);
  static_assert(!concepts::Index<B>);
}

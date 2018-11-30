#include <mln/core/concept/new/points.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

struct A
{
};


struct B
{
};

bool operator==(const B&, const B&)
{
  return true;
}

bool operator!=(const B&, const B&)
{
  return false;
}


struct C : B
{
};

bool operator<(const C&, const C&)
{
  return true;
}
bool operator>(const C&, const C&)
{
  return true;
}

bool operator<=(const C&, const C&)
{
  return true;
}

bool operator>=(const C&, const C&)
{
  return true;
}

TEST(Core, Concept_Point)
{
  static_assert(concepts::Point<int>);
  static_assert(concepts::Point<double>);
  static_assert(!concepts::Point<A>);
  static_assert(!concepts::Point<B>);
  static_assert(concepts::Point<C>);
}

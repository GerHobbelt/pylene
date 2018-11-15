#include <mln/core/concept/new/stl/fundamentals.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts::stl::core;

struct same_test
{
};
struct other
{
};
TEST(Core, Concept_Stl_Same)
{
  static_assert(concepts::Same<same_test, same_test>);
  static_assert(!concepts::Same<same_test, other>);
}

struct base
{
};
struct derived : base
{
};

TEST(Core, Concept_Stl_DerivedFrom)
{
  static_assert(concepts::DerivedFrom<derived, base>);
  static_assert(!concepts::DerivedFrom<other, base>);
}

TEST(Core, Concept_Stl_ConvertibleTo)
{
  static_assert(concepts::ConvertibleTo<int, double>);
  static_assert(concepts::ConvertibleTo<double, int>);
  static_assert(concepts::ConvertibleTo<derived, base>);
}

struct A
{
};
struct B
{
};
struct C : B
{
};
TEST(Core, Concept_Stl_CommonReference)
{
  static_assert(concepts::CommonReference<const int&, double&>);
  static_assert(concepts::CommonReference<double, int>);
  static_assert(!concepts::CommonReference<A, B>);
  static_assert(concepts::CommonReference<B, C>);
}

#include <mln/core/concept/new/domains.hpp>

#include <mln/core/domain/box.hpp>

#include <tuple>
#include <vector>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct domain_test : mln::box2d
{
  using codomain_type = double;

  bool is_discrete() const { return true; }
  bool is_continuous() const { return false; }
};

struct A
{
};

TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<double>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<domain_test>);
}

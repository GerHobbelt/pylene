#include <mln/core/concept/new/domains.hpp>

#include <tuple>
#include <vector>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct domain_test
{
  using point_type    = int;
  using codomain_type = double;

  bool has(const point_type&) const { return true; }
  bool empty() const { return false; }
  bool is_discrete() const { return true; }
  bool is_continuous() const { return false; }
  auto points() const { return ::ranges::view::iota(0, 10); }
};

struct A
{
};

bool operator==(const domain_test&, const domain_test&)
{
  return true;
}

bool operator!=(const domain_test&, const domain_test&)
{
  return false;
}

TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<double>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<domain_test>);
}

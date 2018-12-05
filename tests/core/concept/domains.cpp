#include <mln/core/concept/new/domains.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

struct A
{
};

struct mock_domain
{
  struct mock_point
  {
  };

  using value_type = mock_point;
  using reference  = mock_point&;

  value_type* begin();
  value_type* end();

  bool     has(value_type) const;
  bool     empty() const;
  unsigned size() const;
};

bool operator==(const mock_domain::mock_point&, const mock_domain::mock_point&);
bool operator!=(const mock_domain::mock_point&, const mock_domain::mock_point&);
bool operator<(const mock_domain::mock_point&, const mock_domain::mock_point&);
bool operator<=(const mock_domain::mock_point&, const mock_domain::mock_point&);
bool operator>(const mock_domain::mock_point&, const mock_domain::mock_point&);
bool operator>=(const mock_domain::mock_point&, const mock_domain::mock_point&);


TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<mock_domain>);
}

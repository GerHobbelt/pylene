#include <mln/core/concept/new/domains.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

struct A
{
};

struct mock_domain
{
  struct point_t
  {
  };

  using value_type = point_t;
  using reference  = point_t&;

  point_t* begin();
  point_t* end();

  bool     has(point_t) const;
  bool     empty() const;
  unsigned size() const;
};

bool operator==(const mock_domain::point_t&, const mock_domain::point_t&);
bool operator!=(const mock_domain::point_t&, const mock_domain::point_t&);
bool operator<(const mock_domain::point_t&, const mock_domain::point_t&);
bool operator<=(const mock_domain::point_t&, const mock_domain::point_t&);
bool operator>(const mock_domain::point_t&, const mock_domain::point_t&);
bool operator>=(const mock_domain::point_t&, const mock_domain::point_t&);


TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<mock_domain>);
}

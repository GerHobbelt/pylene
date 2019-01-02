#include <mln/core/concept/new/archetype/domain.hpp>
#include <mln/core/concept/new/domains.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

struct A
{
};


TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<archetypes::Domain>);
}

#include <mln/core/concept/archetype/domain.hpp>
#include <mln/core/concept/domains.hpp>

#include <gtest/gtest.h>


struct A
{
};


TEST(Core, Concept_Domain)
{
  static_assert(not mln::concepts::Domain<int>);
  static_assert(not mln::concepts::Domain<A>);
  static_assert(mln::concepts::Domain<mln::archetypes::Domain>);
}

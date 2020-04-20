#include <mln/core/concepts/archetype/domain.hpp>
#include <mln/core/concepts/domain.hpp>

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

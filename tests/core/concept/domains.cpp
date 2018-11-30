#include <mln/core/concept/new/domains.hpp>

#include <mln/core/domain/box.hpp>

#include <tuple>
#include <vector>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct A
{
};

TEST(Core, Concept_Domain)
{
  static_assert(!concepts::Domain<int>);
  static_assert(!concepts::Domain<double>);
  static_assert(!concepts::Domain<A>);
  static_assert(concepts::Domain<mln::box1d>);
  static_assert(concepts::Domain<mln::box1df>);
  static_assert(concepts::Domain<mln::box2d>);
  static_assert(concepts::Domain<mln::box2df>);
  static_assert(concepts::Domain<mln::box3d>);
  static_assert(concepts::Domain<mln::box3df>);
  static_assert(concepts::Domain<mln::sbox1d>);
  static_assert(concepts::Domain<mln::sbox2d>);
  static_assert(concepts::Domain<mln::sbox3d>);
  static_assert(concepts::Domain<mln::grain_box1d>);
  static_assert(concepts::Domain<mln::grain_box1df>);
  static_assert(concepts::Domain<mln::grain_box2d>);
  static_assert(concepts::Domain<mln::grain_box2df>);
  static_assert(concepts::Domain<mln::grain_box3d>);
  static_assert(concepts::Domain<mln::grain_box3df>);
}

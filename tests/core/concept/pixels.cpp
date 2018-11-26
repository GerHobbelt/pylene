#include <mln/core/concept/new/pixels.hpp>

#include <tuple>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;


struct A
{
};

struct Pix
{
  using value_type = int;
  using point_type = int;
  using reference  = int&;

  point_type point() const { return pnt; }
  reference  value() const { return *val; }

private:
  int  pnt;
  int* val;
};

bool operator==(const Pix& lhs, const Pix& rhs)
{
  return std::tie(lhs.pnt, *lhs.val) == std::tie(rhs.pnt, *rhs.val);
}

bool operator!=(const Pix& lhs, const Pix& rhs)
{
  return !(lhs == rhs);
}

TEST(Core, Concept_Pixel)
{
  static_assert(!concepts::Pixel<int>);
  static_assert(!concepts::Pixel<A>);
  static_assert(concepts::Pixel<Pix>);
}

#include <mln/core/concept/new/values.hpp>

#include <tuple>

#include <gtest/gtest.h>


namespace concepts = mln::core::concepts;

struct RGB
{
  RGB(int r, int g, int b)
    : r(r)
    , g(g)
    , b(b)
  {
  }

  RGB() = default;

  RGB(const RGB&) = default;
  RGB& operator=(const RGB&) = default;

  RGB(RGB&&)   = default;
  RGB& operator=(RGB&&) = default;

  int r, g, b;
};

struct Empty
{
};

TEST(Core, Concept_Value)
{
  static_assert(concepts::Value<int>);
  static_assert(concepts::Value<RGB>);
  static_assert(concepts::Value<Empty>);
}

struct RGBA : RGB
{
  int a;
};

bool operator==(const RGBA& lhs, const RGBA& rhs)
{
  return std::tie(lhs.r, lhs.g, lhs.b, lhs.a) == std::tie(rhs.r, rhs.g, rhs.b, rhs.a);
}
bool operator!=(const RGBA& lhs, const RGBA& rhs)
{
  return !(lhs == rhs);
}

TEST(Core, Concept_ComparableValue)
{
  static_assert(concepts::ComparableValue<int>);
  static_assert(!concepts::ComparableValue<RGB>);
  static_assert(!concepts::ComparableValue<Empty>);
  static_assert(concepts::ComparableValue<RGBA>);
}


struct RGBAB : RGBA
{
  int b;
};

bool operator<(const RGBAB& lhs, const RGBAB& rhs)
{
  return std::tie(lhs.r, lhs.g, lhs.b, lhs.a, lhs.b) < std::tie(rhs.r, rhs.g, rhs.b, rhs.a, rhs.b);
}
bool operator>(const RGBAB& lhs, const RGBAB& rhs)
{
  return rhs < lhs;
}
bool operator<=(const RGBAB& lhs, const RGBAB& rhs)
{
  return !(lhs > rhs);
}
bool operator>=(const RGBAB& lhs, const RGBAB& rhs)
{
  return !(lhs < rhs);
}

TEST(Core, Concept_OrderedValue)
{
  static_assert(concepts::OrderedValue<int>);
  static_assert(!concepts::OrderedValue<RGB>);
  static_assert(!concepts::OrderedValue<Empty>);
  static_assert(!concepts::OrderedValue<RGBA>);
  static_assert(concepts::OrderedValue<RGBAB>);
}

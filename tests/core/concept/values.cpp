#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/values.hpp>

#include <tuple>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

struct Empty
{
};

TEST(Core, Concept_Value)
{
  static_assert(concepts::Value<int>);
  static_assert(concepts::Value<Empty>);
  static_assert(concepts::Value<archetypes::Value>);
}

TEST(Core, Concept_ComparableValue)
{
  static_assert(concepts::ComparableValue<int>);
  static_assert(!concepts::ComparableValue<archetypes::Value>);
  static_assert(!concepts::ComparableValue<Empty>);
  static_assert(concepts::ComparableValue<archetypes::ComparableValue>);
}

TEST(Core, Concept_OrderedValue)
{
  static_assert(concepts::OrderedValue<int>);
  static_assert(!concepts::OrderedValue<archetypes::Value>);
  static_assert(!concepts::OrderedValue<Empty>);
  static_assert(!concepts::OrderedValue<archetypes::ComparableValue>);
  static_assert(concepts::OrderedValue<archetypes::OrderedValue>);
}

#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/values.hpp>

#include <tuple>

#include <gtest/gtest.h>


struct Empty
{
};

TEST(Core, Concept_Value)
{
  static_assert(mln::concepts::Value<int>);
  static_assert(mln::concepts::Value<Empty>);
  static_assert(mln::concepts::Value<mln::archetypes::Value>);
}

TEST(Core, Concept_ComparableValue)
{
  static_assert(mln::concepts::ComparableValue<int>);
  static_assert(not mln::concepts::ComparableValue<mln::archetypes::Value>);
  static_assert(not mln::concepts::ComparableValue<Empty>);
  static_assert(mln::concepts::ComparableValue<mln::archetypes::ComparableValue>);
}

TEST(Core, Concept_OrderedValue)
{
  static_assert(mln::concepts::OrderedValue<int>);
  static_assert(not mln::concepts::OrderedValue<mln::archetypes::Value>);
  static_assert(not mln::concepts::OrderedValue<Empty>);
  static_assert(not mln::concepts::OrderedValue<mln::archetypes::ComparableValue>);
  static_assert(mln::concepts::OrderedValue<mln::archetypes::OrderedValue>);
}

#include <mln/core/concept/new/archetype/range.hpp>
#include <mln/core/concept/new/ranges.hpp>

#include <gtest/gtest.h>


TEST(Core, Concept_SegmentedRange)
{
  static_assert(mln::concepts::SegmentedRange<mln::archetypes::SegmentedRange>);
  static_assert(not mln::concepts::SegmentedRange<mln::archetypes::ReversibleRange>);
  static_assert(mln::concepts::SegmentedRange<mln::archetypes::SegmentedAndReversibleRange>);
}

TEST(Core, Concept_ReversibleRange)
{
  static_assert(not mln::concepts::ReversibleRange<mln::archetypes::SegmentedRange>);
  static_assert(mln::concepts::ReversibleRange<mln::archetypes::ReversibleRange>);
  static_assert(mln::concepts::ReversibleRange<mln::archetypes::SegmentedAndReversibleRange>);
}

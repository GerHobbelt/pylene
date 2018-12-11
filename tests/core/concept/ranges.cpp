#include <mln/core/concept/new/archetype/range.hpp>
#include <mln/core/concept/new/ranges.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;


TEST(Core, Concept_SegmentedRange)
{
  static_assert(concepts::SegmentedRange<archetypes::SegmentedRange>);
  static_assert(!concepts::SegmentedRange<archetypes::ReversibleRange>);
  static_assert(concepts::SegmentedRange<archetypes::SegmentedAndReversibleRange>);
}

TEST(Core, Concept_ReversibleRange)
{
  static_assert(!concepts::ReversibleRange<archetypes::SegmentedRange>);
  static_assert(concepts::ReversibleRange<archetypes::ReversibleRange>);
  static_assert(concepts::ReversibleRange<archetypes::SegmentedAndReversibleRange>);
}

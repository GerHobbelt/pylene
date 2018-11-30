#include <mln/core/concept/new/ranges.hpp>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

struct mock_randomaccess_rng
{
  using value_type = int;
  using reference  = int&;

  int* begin();
  int* end();
};

static_assert(concepts::stl::RandomAccessRange<mock_randomaccess_rng>);

struct mock_segmented_range : mock_randomaccess_rng
{
  mock_randomaccess_rng rows();
};

struct mock_reversible_range : mock_randomaccess_rng
{
  mock_randomaccess_rng reversed();
};

struct mock_segmented_and_reversible_range : mock_randomaccess_rng
{
  mock_randomaccess_rng rows();
  mock_randomaccess_rng reversed();
};


TEST(Core, Concept_SegmentedRange)
{
  static_assert(!concepts::SegmentedRange<mock_randomaccess_rng>);
  static_assert(concepts::SegmentedRange<mock_segmented_range>);
  static_assert(!concepts::SegmentedRange<mock_reversible_range>);
  static_assert(concepts::SegmentedRange<mock_segmented_and_reversible_range>);
}

TEST(Core, Concept_ReversibleRange)
{
  static_assert(!concepts::ReversibleRange<mock_randomaccess_rng>);
  static_assert(!concepts::ReversibleRange<mock_segmented_range>);
  static_assert(concepts::ReversibleRange<mock_reversible_range>);
  static_assert(concepts::ReversibleRange<mock_segmented_and_reversible_range>);
}

#include <mln/core/concept/new/structuring_elements.hpp>
#include <mln/core/se/periodic_line2d.hpp>

#include <gtest/gtest.h>

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::StructuringElement<mln::experimental::se::periodic_line2d, mln::point2d>);
#endif


TEST(Core, periodic_line2d)
{
  mln::point2d p      = {-1, 1};
  auto         line2d = mln::experimental::se::periodic_line2d{p, 2};

  EXPECT_EQ(5, line2d.size());
  EXPECT_EQ(2, line2d.repetition());
  EXPECT_EQ((mln::point2d{1, -1}), line2d.period());

  std::vector<mln::point2d> expected_offsets        = {{-2, 2}, {-1, 1}, {0, 0}, {1, -1}, {2, -2}};
  std::vector<mln::point2d> expected_before_offsets = {{-2, 2}, {-1, 1}};
  std::vector<mln::point2d> expected_after_offsets  = {{1, -1}, {2, -2}};

  EXPECT_EQ(expected_offsets, ::ranges::to_vector(line2d(mln::point2d{0, 0})));
  EXPECT_EQ(expected_before_offsets, ::ranges::to_vector(line2d.before(mln::point2d{0, 0})));
  EXPECT_EQ(expected_after_offsets, ::ranges::to_vector(line2d.after(mln::point2d{0, 0})));
}

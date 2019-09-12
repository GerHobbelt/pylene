#include <gtest/gtest.h>
#include <mln/core/se/mask2d.hpp>

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::StructuringElement<mln::se::experimental::mask2d, mln::experimental::point2d>);
#endif

TEST(Core, Mask2d)
{
  mln::se::experimental::mask2d mask = {{1, 0, 0}, {1, 1, 1}, {0,0,0}};
  std::vector<mln::experimental::point2d> delta = {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}};

  int i = 0;
  for (auto n : mask(mln::experimental::point2d{0,0}))
    EXPECT_EQ(delta[i++], n);

  EXPECT_EQ(1, mask.radial_extent());
}


TEST(Core, Mask2d_Invalid_Dimension)
{
  EXPECT_ANY_THROW(auto mask = mln::se::experimental::mask2d({{1, 0, 0}, {1, 1, 1}}));
}

#include <gtest/gtest.h>
#include <mln/core/se/mask2d.hpp>

#include <mln/core/image/experimental/ndimage.hpp>

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


TEST(Core, Weighted_Mask2d)
{
  mln::se::experimental::wmask2d mask = //
      {{1.5f, 1.0f, 0.0f},              //
       {1.0f, 0.0f, 1.0f},              //
       {0.0f, 1.0f, 1.5f}};


  std::vector<mln::experimental::point2d> delta = {{-1, -1}, {0, -1}, {-1, 0}, {1, 0}, {0, 1}, {1, 1}};

  int i = 0;
  mln::experimental::point2d center = {0,0};
  for (auto [n,w] : mask(center))
  {
    EXPECT_EQ(delta[i], n);
    if (std::abs(delta[i].x()) + std::abs(delta[i].y()) == 1)
      EXPECT_EQ(1.f, w);
    else
      EXPECT_EQ(1.5f, w);
    i++;
  }

  EXPECT_EQ(1, mask.radial_extent());
}

TEST(Core, Weighted_Mask2d_2)
{
  mln::se::experimental::wmask2d mask = //
      {{1.5f, 1.0f, 0.0f},              //
       {1.0f, 0.0f, 1.0f},              //
       {0.0f, 1.0f, 1.5f}};

  mln::experimental::image2d<int> f = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};


  std::vector<mln::experimental::point2d> delta = {{0, 0}, {1, 0}, {0, 1}, {2, 1}, {1, 2}, {2, 2}};

  int i = 0;
  mln::experimental::point2d center = {1,1};
  auto px = f.new_pixel(center);

  for (auto wpx : mask(px))
  {
    EXPECT_EQ(delta[i], wpx.point());
    EXPECT_EQ(f(delta[i]), wpx.val());
    if (std::abs(delta[i].x() - 1) + std::abs(delta[i].y() - 1) == 1)
      EXPECT_EQ(1.f, wpx.weight());
    else
      EXPECT_EQ(1.5f, wpx.weight());
    i++;
  }

  EXPECT_EQ(1, mask.radial_extent());
}



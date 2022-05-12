#include <mln/core/neighborhood/c8.hpp>
#include <mln/morpho/waterfall.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

#include <vector>

TEST(Morpho, waterfall_c4)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 1, 0}, //
      {3, 3, 3, 2, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };
  const mln::image2d<int> nodemap_ref{
      {4, 4, 4, 4, 3}, //
      {4, 4, 4, 4, 3}, //
      {4, 4, 4, 1, 3}, //
      {2, 2, 2, 1, 1}, //
      {2, 2, 2, 1, 1}  //
  };
  std::vector<int> parent_ref{0, 0, 0, 0, 0};
  std::vector<int> values_ref{1, 0, 0, 0, 0};

  const auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c4);

  ASSERT_IMAGES_EQ_EXP(nodemap, nodemap_ref);
  ASSERT_EQ(t.parent, parent_ref);
  ASSERT_EQ(t.values, values_ref);
}

TEST(Morpho, waterfall_c8)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 2, 0}, //
      {3, 3, 3, 1, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<int> nodemap_ref{
      {4, 4, 4, 4, 3}, //
      {4, 4, 4, 4, 3}, //
      {4, 4, 4, 4, 3}, //
      {2, 2, 2, 1, 1}, //
      {2, 2, 2, 1, 1}  //
  };
  std::vector<int> parent_ref{0, 0, 0, 0, 0};
  std::vector<int> values_ref{1, 0, 0, 0, 0};

  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

  ASSERT_IMAGES_EQ_EXP(nodemap, nodemap_ref);
  ASSERT_EQ(t.parent, parent_ref);
  ASSERT_EQ(t.values, values_ref);
}
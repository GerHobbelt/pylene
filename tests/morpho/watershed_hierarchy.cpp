#include "mln/morpho/watershed_hierarchy.hpp"
#include <mln/accu/accumulators/count.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>

#include <gtest/gtest.h>

TEST(Morpho, AreaWatershedHierarchyGray)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<int> expected_parent = {0,  0,  1,  2,  2,  3, 5,  0,  6,  1,  9,  4,  3,  11, 7,  14, 15,
                                      16, 8,  18, 19, 20, 4, 21, 7,  14, 15, 16, 17, 9,  10, 10, 5,  17,
                                      18, 19, 20, 21, 12, 6, 11, 13, 23, 12, 22, 22, 13, 23, 8};

  std::vector<unsigned long> expected_values = {6, 3, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::accu::features::count<>(), mln::c4,
      [](const auto& a, const auto& b) -> float { return mln::functional::l2dist_t<>()(a, b); });

  ASSERT_EQ(expected_parent.size(), tree.parent.size());
  ASSERT_EQ(expected_values.size(), tree.values.size());
  ASSERT_EQ(tree.parent.size(), tree.values.size());

  for (std::size_t i = 0; i < expected_parent.size(); ++i)
  {
    EXPECT_EQ(expected_parent[i], tree.parent[i]);
    EXPECT_EQ(expected_values[i], tree.values[i]);
  }
}

TEST(Morpho, AreaWatershedHierarchyRGB)
{
  mln::image2d<mln::rgb8> input = {
      {{255, 146, 0}, {31, 174, 0}, {45, 0, 255}, {255, 75, 0}, {31, 174, 0}}, //
      {{45, 0, 255}, {255, 75, 0}, {3, 0, 0}, {45, 0, 255}, {255, 255, 255}},  //
      {{3, 0, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 75, 0}},     //
      {{31, 174, 0}, {255, 255, 255}, {45, 0, 255}, {3, 0, 0}, {45, 0, 255}},  //
      {{255, 75, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 146, 0}}, //
  };

  std::vector<int> expected_parent = {0,  0,  0,  1,  1,  4,  5,  6,  2,  3,  8,  9,  11, 12, 7, 13, 2,
                                      3,  14, 17, 18, 20, 15, 22, 18, 20, 11, 12, 13, 6,  21, 7, 4,  22,
                                      14, 21, 15, 23, 23, 5,  8,  10, 17, 9,  16, 16, 10, 19, 19};

  std::vector<unsigned long> expected_values = {3, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::accu::features::count<>(), mln::c4,
      [](const auto& a, const auto& b) -> float { return mln::functional::l2dist_t<>()(a, b); });

  ASSERT_EQ(expected_parent.size(), tree.parent.size());
  ASSERT_EQ(expected_values.size(), tree.values.size());
  ASSERT_EQ(tree.parent.size(), tree.values.size());

  for (std::size_t i = 0; i < expected_parent.size(); ++i)
  {
    EXPECT_EQ(expected_parent[i], tree.parent[i]);
    EXPECT_EQ(expected_values[i], tree.values[i]);
  }
}
#include "mln/morpho/watershed_hierarchy.hpp"
#include <mln/accu/accumulators/count.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

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

  std::vector<int> expected_parent = {0,  0,  1,  1, 0,  4, 5,  6,  7,  1, 9,  10, 2,  8, 12,
                                      14, 15, 16, 2, 14, 3, 20, 17, 18, 3, 24, 21, 22, 14};

  std::vector<unsigned long> expected_values = {6, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

TEST(Morpho, AreaWatershedHierarchyGrayHQ)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<int> expected_parent = {0,  0,  1,  1, 0,  4, 5,  6,  7,  1, 9,  10, 2,  8, 12,
                                      14, 15, 16, 2, 14, 3, 20, 17, 18, 3, 24, 21, 22, 14};

  std::vector<unsigned long> expected_values = {6, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::accu::features::count<>(), mln::c4,
      [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); });

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

  std::vector<int> expected_parent = {0, 0,  0,  0,  3, 2, 5,  6, 3, 4, 3,  3,  7,  3,
                                      9, 12, 12, 16, 3, 1, 19, 2, 5, 1, 23, 20, 21, 26};

  std::vector<unsigned long> expected_values = {3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

TEST(Morpho, AreaWatershedHierarchyGrayC8)
{
  mln::image2d<std::uint8_t> input = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };

  std::vector<int> expected_parent = {0, 0,  0, 0,  1, 3, 2,  3,  1,  4,  6, 5,  7,  8,  9,
                                      2, 11, 4, 13, 4, 5, 15, 21, 17, 19, 5, 21, 22, 17, 23};

  std::vector<unsigned long> expected_values = {7, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::accu::features::count<>(), mln::c8,
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

TEST(Morpho, AreaWatershedHierarchy3DImage)
{
  const mln::image3d<uint8_t> input = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  std::vector<int> expected_parent = {0, 0,  1,  0, 3, 3,  2, 2, 6, 1,  5,  8,  8,  7,  9, 9,
                                      4, 10, 12, 8, 9, 18, 9, 8, 1, 17, 25, 24, 26, 28, 27};

  std::vector<unsigned long> expected_values = {9, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::accu::features::count<>(), mln::c26,
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
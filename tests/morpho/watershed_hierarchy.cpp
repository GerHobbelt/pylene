#include "mln/morpho/watershed_hierarchy.hpp"
#include <mln/accu/accumulators/count.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <gtest/gtest.h>

TEST(Morpho, HeightAttribute)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<float> expected_attribute = {
      134.f, 134.f, 125.f, 70.f, 70.f, 70.f, 70.f, 70.f, 61.f, 38.f, 0.f, 29.f, 38.f, 0.f, 42.f, 0.f, 0.f,
      0.f,   42.f,  0.f,   0.f,  0.f,  70.f, 0.f,  0.f,  0.f,  0.f,  0.f, 0.f,  0.f,  0.f, 0.f,  0.f, 0.f,
      0.f,   0.f,   0.f,   0.f,  0.f,  0.f,  0.f,  0.f,  0.f,  0.f,  0.f, 0.f,  0.f,  0.f, 0.f};

  // Build BPT
  auto [tree, nm] = mln::morpho::internal::__alphatree<false>(
      input, mln::c4, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); }, false,
      false);

  auto attribute = mln::morpho::internal::height_attribute(tree, nm);

  ASSERT_EQ(expected_attribute.size(), attribute.size());

  for (std::size_t i = 0; i < expected_attribute.size(); ++i)
    EXPECT_EQ(expected_attribute[i], attribute[i]);
}

TEST(Morpho, DynamicAttribute)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  // Build BPT
  auto [tree, nm] = mln::morpho::internal::__alphatree<false>(
      input, mln::c4, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); }, false,
      false);

  std::vector<bool> expected_extrema_attribute = {false, false, false, false, false, false, false, false, false, true,
                                                  false, true,  true,  false, true,  false, false, false, true,  false,
                                                  false, false, true,  false, false, false, false, false, false, false,
                                                  false, false, false, false, false, false, false, false, false, false,
                                                  false, false, false, false, false, false, false, false, false};

  auto extrema_attribute = mln::morpho::internal::extrema_attribute(tree, nm);

  ASSERT_EQ(expected_extrema_attribute.size(), extrema_attribute.size());

  for (std::size_t i = 0; i < expected_extrema_attribute.size(); ++i)
    EXPECT_EQ(expected_extrema_attribute[i], extrema_attribute[i]);

  std::vector<float> expected_dynamic_attribute = {
      134.f, 134.f, 134.f, 70.f, 70.f, 70.f, 70.f, 134.f, 70.f,  38.f,  38.f,  29.f,  38.f, 29.f, 134.f, 134.f, 134.f,
      134.f, 70.f,  70.f,  70.f, 70.f, 70.f, 70.f, 0,     134.f, 134.f, 134.f, 134.f, 38.f, 38.f, 38.f,  0,     134.f,
      70.f,  70.f,  70.f,  70.f, 38.f, 0,    29.f, 29.f,  70.f,  38.f,  70.f,  70.f,  29.f, 70.f, 0};

  auto dynamic_attribute = mln::morpho::internal::dynamic_attribute(tree, nm);

  ASSERT_EQ(expected_dynamic_attribute.size(), dynamic_attribute.size());

  for (std::size_t i = 0; i < expected_dynamic_attribute.size(); ++i)
    EXPECT_EQ(expected_dynamic_attribute[i], dynamic_attribute[i]);
}

TEST(Morpho, AreaWatershedHierarchyGray)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<int>           expected_parent = {0, 0, 1, 1, 0, 1, 2, 2, 3, 3};
  std::vector<unsigned long> expected_values = {6, 3, 2, 2, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c4, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

  ASSERT_EQ(expected_parent.size(), tree.parent.size());
  ASSERT_EQ(expected_values.size(), tree.values.size());
  ASSERT_EQ(tree.parent.size(), tree.values.size());

  for (std::size_t i = 0; i < expected_parent.size(); ++i)
  {
    EXPECT_EQ(expected_parent[i], tree.parent[i]);
    EXPECT_EQ(expected_values[i], tree.values[i]);
  }
}

TEST(Morpho, DynamicWatershedHierarchyGray)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<int>           expected_parent = {0, 0, 1, 0, 1, 2, 1, 2, 0};
  std::vector<unsigned long> expected_values = {70, 38, 29, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::morpho::WatershedAttribute::DYNAMIC, mln::c4,
      [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

  ASSERT_EQ(expected_parent.size(), tree.parent.size());
  ASSERT_EQ(expected_values.size(), tree.values.size());
  ASSERT_EQ(tree.parent.size(), tree.values.size());

  for (std::size_t i = 0; i < expected_parent.size(); ++i)
  {
    EXPECT_EQ(expected_parent[i], tree.parent[i]);
    EXPECT_EQ(expected_values[i], tree.values[i]);
  }
}

TEST(Morpho, HeightWatershedHierarchyGray)
{
  mln::image2d<uint8_t> input = {
      {163, 112, 42, 121, 112},  //
      {42, 121, 1, 42, 255},     //
      {1, 112, 121, 112, 121},   //
      {112, 255, 42, 1, 42},     //
      {121, 112, 121, 112, 163}, //
  };

  std::vector<int>           expected_parent = {0, 0, 1, 0, 1, 2, 1, 2, 0};
  std::vector<unsigned long> expected_values = {70, 38, 29, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input, mln::morpho::WatershedAttribute::HEIGHT, mln::c4,
      [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

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

  std::vector<int>           expected_parent = {0, 0, 1, 1, 0, 1, 2, 2, 3, 3};
  std::vector<unsigned long> expected_values = {6, 3, 2, 2, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c4, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

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

  std::vector<int>           expected_parent = {0, 0, 0, 0, 2, 1, 2, 1};
  std::vector<unsigned long> expected_values = {3, 2, 2, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c4, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

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

  std::vector<int>           expected_parent = {0, 0, 0, 0, 1, 3, 2, 3, 1, 4, 2, 4, 4};
  std::vector<unsigned long> expected_values = {7, 3, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c8, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

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

  std::vector<int>           expected_parent = {0, 0, 1, 0, 3, 3, 2, 2, 1, 1};
  std::vector<unsigned long> expected_values = {9, 3, 2, 2, 0, 0, 0, 0, 0, 0};

  auto [tree, _] = mln::morpho::watershed_hierarchy(
      input,
      [](auto tree, auto nm) -> std::vector<size_t> {
        return tree.compute_attribute_on_points(nm, mln::accu::features::count<>());
      },
      mln::c26, [](const auto& a, const auto& b) -> float { return mln::functional::l2dist(a, b); });

  ASSERT_EQ(expected_parent.size(), tree.parent.size());
  ASSERT_EQ(expected_values.size(), tree.values.size());
  ASSERT_EQ(tree.parent.size(), tree.values.size());

  for (std::size_t i = 0; i < expected_parent.size(); ++i)
  {
    EXPECT_EQ(expected_parent[i], tree.parent[i]);
    EXPECT_EQ(expected_values[i], tree.values[i]);
  }
}
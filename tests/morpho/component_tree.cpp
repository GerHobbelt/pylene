#include <gtest/gtest.h>
#include <iostream>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree.hpp>
#include <mln/morpho/tos.hpp>

TEST(Component_tree, Saliency_Map)
{
  mln::image2d<uint8_t> img = {{0, 0, 100, 0}, {0, 0, 75, 0}, {95, 80, 60, 0}, {0, 0, 0, 0}};

  int expected_s_map_val[24][3] = {{0, 1, 0},    {0, 4, 0},    {1, 2, 100}, {1, 5, 0},   {2, 3, 100}, {2, 6, 25},
                                   {3, 7, 0},    {4, 5, 0},    {4, 8, 95},  {5, 6, 75},  {5, 9, 80},  {6, 7, 75},
                                   {6, 10, 15},  {7, 11, 0},   {8, 9, 15},  {8, 12, 95}, {9, 10, 20}, {9, 13, 80},
                                   {10, 11, 60}, {10, 14, 60}, {11, 15, 0}, {12, 13, 0}, {13, 14, 0}, {14, 15, 0}};

  auto [t, node_map] = mln::morpho::tos(img, {0, 0});

  const std::vector<mln::morpho::Edge>& s_map = mln::morpho::saliency_map(img);

  for (int i = 0; i < 24; ++i)
  {
    auto expected_edge = expected_s_map_val[i];

    ASSERT_EQ(expected_edge[2], std::get<2>(s_map[i]));
  }
}


TEST(Component_tree, Saliency)
{
  mln::image2d<uint8_t> img = {{0, 0, 100, 0}, {0, 0, 75, 0}, {95, 80, 60, 0}, {0, 0, 0, 0}};

  int expected_saliency[9 * 9] = {255, 255, 255, 255, 155, 255, 155, 255, 255, //
                                  255, 255, 255, 255, 155, 255, 155, 255, 255, //
                                  255, 255, 255, 255, 155, 230, 155, 255, 255, //
                                  255, 255, 255, 255, 180, 255, 180, 255, 255, //
                                  160, 160, 160, 175, 175, 240, 180, 255, 255, //
                                  255, 255, 240, 255, 235, 255, 195, 255, 255, //
                                  160, 160, 160, 175, 175, 195, 195, 255, 255, //
                                  255, 255, 255, 255, 255, 255, 255, 255, 255, //
                                  255, 255, 255, 255, 255, 255, 255, 255, 255};


  auto [t, node_map] = mln::morpho::tos(img, {0, 0});

  auto saliency = t.saliency(img);

  for (int i = 0; i < 9 * 9; ++i)
  {
    ASSERT_EQ(expected_saliency[i], static_cast<int>(saliency({i % 9, i / 9})));
  }
}
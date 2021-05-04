#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/cut.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

template <typename V>
mln::image2d<int> //
cut(const mln::morpho::component_tree<V>& t, mln::image2d<int>& node_map, int alpha)
{
  std::size_t n = t.parent.size();

  std::vector<int> labels(n, -1);

  int nlabel = 0;
  {
    mln_foreach (auto px, node_map.pixels())
      if (labels[px.val()] == -1)
        labels[px.val()] = nlabel++;
  }

  for (std::size_t i = n - 1; i > 0; --i)
    if (t.values[t.parent[i]] <= alpha) // progate to parent
      if (labels[t.parent[i]] == -1)
        labels[t.parent[i]] = labels[i];

  // Propagation downward
  for (std::size_t i = n - 1; i > 0; --i)
    if (t.values[t.parent[i]] <= alpha) // progate to parent
      labels[i] = labels[t.parent[i]];

  mln::image2d<int> imlabel;
  imlabel.resize(node_map.domain());

  mln_foreach (auto px, imlabel.pixels())
    px.val() = labels[node_map(px.point())];

  return imlabel;
}


TEST(Morpho, AlphaTree)
{
  mln::image2d<uint8_t> ima = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };
  auto [t, node_map] = mln::morpho::alphatree(ima, mln::c4);

  mln::image2d<int> ref0 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref1 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 16, 17, 13, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref10 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  mln::image2d<int> ref11 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 4},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 0), ref0);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 1), ref1);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 10), ref10);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 11), ref11);
}

TEST(Morpho, AlphaTreeRGB8)
{
  const mln::image2d<mln::rgb8> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  const mln::image2d<int> ref_8 = {
      {6, 6, 13},  //
      {12, 6, 10}, //
      {9, 8, 7}    //
  };

  const mln::image2d<int> ref_16 = {
      {6, 6, 13}, //
      {5, 6, 10}, //
      {5, 8, 7}   //
  };

  const mln::image2d<int> ref_17 = {
      {6, 6, 13}, //
      {4, 6, 10}, //
      {4, 4, 7}   //
  };

  const mln::image2d<int> ref_18 = {
      {3, 3, 13}, //
      {3, 3, 10}, //
      {3, 3, 7}   //
  };

  const mln::image2d<int> ref_24 = {
      {2, 2, 13}, //
      {2, 2, 10}, //
      {2, 2, 2}   //
  };

  const mln::image2d<int> ref_32 = {
      {1, 1, 13}, //
      {1, 1, 1},  //
      {1, 1, 1}   //
  };

  const mln::image2d<int> ref_36 = {
      {0, 0, 0}, //
      {0, 0, 0}, //
      {0, 0, 0}  //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> int { return mln::functional::l2dist_t<>()(a, b); });
  auto rng = std::vector<int>(t.parent.size());
  std::iota(rng.begin(), rng.end(), 0);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 0, rng), nm);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 8, rng), ref_8);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 16, rng), ref_16);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 17, rng), ref_17);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 18, rng), ref_18);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 24, rng), ref_24);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 32, rng), ref_32);
  ASSERT_IMAGES_EQ_EXP(mln::morpho::horizontal_cut_labelization_from(t, nm, 36, rng), ref_36);
}
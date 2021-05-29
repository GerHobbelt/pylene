#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/morpho/alphatree.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

template <typename V, typename I, typename T>
I cut(const mln::morpho::component_tree<V>& t, I& node_map, T alpha)
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

  I imlabel;
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
  auto [t, node_map] = mln::morpho::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref_1 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 16, 17, 13, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref_10 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 9},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  mln::image2d<int> ref_11 = {
      {0, 0, 2, 3, 4},      //
      {5, 0, 2, 8, 4},      //
      {5, 11, 12, 13, 14},  //
      {15, 11, 17, 13, 19}, //
      {15, 20, 17, 22, 23}, //
  };


  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 0u), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 1u), ref_1);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 10u), ref_10);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 11u), ref_11);
}

TEST(Morpho, AlphaTreeParentRelation)
{
  mln::image2d<int> ima = {
      {4, 0, 0, 1},  //
      {5, 0, 8, 1},  //
      {6, 0, 9, 1},  //
      {7, 0, 10, 1}, //
      {2, 3, 4, 5},  //
  };

  auto [t, _] = mln::morpho::alphatree(ima, mln::c4);

  for (std::size_t i = 0; i < t.parent.size(); ++i)
    ASSERT_TRUE(static_cast<int>(i) >= t.parent[i]);
}

TEST(Morpho, AlphaTreeCanonized)
{
  mln::image2d<int> ima = {
      {4, 0, 0, 1},  //
      {5, 0, 8, 1},  //
      {6, 0, 9, 1},  //
      {7, 0, 10, 1}, //
      {2, 3, 4, 5},  //
  };

  auto [t, _] = mln::morpho::alphatree(ima, mln::c4);

  for (std::size_t i = 1; i < t.parent.size(); ++i)
    ASSERT_TRUE(t.values[i] != t.values[t.parent[i]]);
}

TEST(Morpho, AlphaTreeMST)
{
  using I = mln::image2d<std::uint8_t>;
  using P = mln::image_point_t<I>;
  using W = std::uint8_t;
  using E = mln::morpho::internal::edge_t<P, W>;

  I ima = {
      {4, 0, 0, 1},  //
      {5, 0, 8, 1},  //
      {6, 0, 9, 1},  //
      {7, 0, 10, 1}, //
      {2, 3, 4, 5},  //
  };

  std::vector<E> expected_mst = {
      {{2, 4}, {3, 4}, 1}, //
      {{1, 4}, {2, 4}, 1}, //
      {{0, 4}, {1, 4}, 1}, //
      {{2, 0}, {3, 0}, 1}, //
      {{2, 2}, {2, 3}, 1}, //
      {{0, 2}, {0, 3}, 1}, //
      {{2, 1}, {2, 2}, 1}, //
      {{0, 1}, {0, 2}, 1}, //
      {{0, 0}, {0, 1}, 1}, //
      {{1, 3}, {1, 4}, 3}, //
      {{0, 0}, {1, 0}, 4}, //
      {{2, 3}, {2, 4}, 6}  //
  };

  std::vector<E> mst;

  auto [t, _] = mln::morpho::internal::__alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> W { return mln::functional::l2dist_t<>()(a, b); }, true, &mst);

  for (std::size_t i = 0; i < expected_mst.size(); ++i)
  {
    ASSERT_EQ(expected_mst[i].p, mst[i].p);
    ASSERT_EQ(expected_mst[i].q, mst[i].q);
    ASSERT_EQ(expected_mst[i].w, mst[i].w);
  }
  ASSERT_EQ(expected_mst.size(), mst.size());
}

TEST(Morpho, AlphaTreeRGB8Uint16Distance)
{
  const mln::image2d<mln::rgb8> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::uint16_t { return mln::functional::l2dist_t<>()(a, b); });

  const mln::image2d<int> ref_0 = {
      {0, 0, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_8 = {
      {0, 0, 1}, //
      {2, 0, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_16 = {
      {0, 0, 1}, //
      {2, 0, 4}, //
      {2, 6, 7}  //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0u), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 8u), ref_8);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16u), ref_16);
}

TEST(Morpho, AlphaTreeRGB8FloatDistance)
{
  const mln::image2d<mln::rgb8> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::float_t { return mln::functional::l2dist_t<>()(a, b); });

  const mln::image2d<int> ref_0 = {
      {0, 0, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_16 = {
      {0, 0, 1}, //
      {2, 0, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_32 = {
      {0, 0, 1}, //
      {2, 0, 4}, //
      {2, 6, 7}  //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0.f), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16.f), ref_16);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 32.f), ref_32);
}

TEST(Morpho, AlphaTree8C)
{
  mln::image2d<std::uint32_t> ima = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c8, [](const auto& a, const auto& b) -> std::uint32_t { return mln::functional::l2dist_t<>()(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 0, 6, 7, 8},      //
      {9, 10, 11, 12, 13},  //
      {14, 15, 16, 17, 18}, //
      {14, 19, 20, 21, 22}, //
  };

  mln::image2d<int> ref_16 = {
      {0, 0, 2, 2, 4},     //
      {5, 0, 6, 7, 8},     //
      {9, 10, 11, 8, 7},   //
      {9, 10, 16, 16, 18}, //
      {9, 19, 20, 18, 22}, //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0.f), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16.f), ref_16);
}

TEST(Morpho, AlphaTree8CHQUEUE)
{
  mln::image2d<std::uint8_t> ima = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c8, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 0, 6, 7, 8},      //
      {9, 10, 11, 12, 13},  //
      {14, 15, 16, 17, 18}, //
      {14, 19, 20, 21, 22}, //
  };

  mln::image2d<int> ref_16 = {
      {0, 1, 2, 3, 4},      //
      {5, 0, 2, 7, 8},      //
      {5, 10, 0, 12, 13},   //
      {14, 10, 16, 12, 18}, //
      {14, 19, 16, 18, 22}, //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0.f), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16.f), ref_16);
}

TEST(Morpho, AlphaTree3DImage)
{
  const mln::image3d<uint32_t> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint32_t { return mln::functional::l2dist_t<>()(a, b); });

  const mln::image3d<int> ref_0 = {
      {{0, 1, 1}, {0, 1, 1}, {2, 3, 4}},         //
      {{5, 6, 7}, {8, 9, 10}, {11, 12, 13}},     //
      {{14, 15, 16}, {12, 17, 18}, {19, 20, 21}} //
  };

  const mln::image3d<int> ref_16 = {
      {{0, 1, 1}, {0, 1, 1}, {1, 3, 4}},     //
      {{5, 6, 7}, {8, 9, 10}, {11, 0, 13}},  //
      {{5, 15, 9}, {0, 15, 13}, {19, 9, 10}} //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0u), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16u), ref_16);
}

TEST(Morpho, AlphaTree3DImageHQUEUE)
{
  const mln::image3d<uint8_t> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  auto [t, nm] = mln::morpho::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist_t<>()(a, b); });

  const mln::image3d<int> ref_0 = {
      {{0, 1, 1}, {0, 1, 1}, {2, 3, 4}},         //
      {{5, 6, 7}, {8, 9, 10}, {11, 12, 13}},     //
      {{14, 15, 16}, {12, 17, 18}, {19, 20, 21}} //
  };

  const mln::image3d<int> ref_16 = {
      {{0, 1, 1}, {0, 1, 1}, {0, 3, 4}},    //
      {{5, 6, 7}, {5, 1, 10}, {11, 6, 13}}, //
      {{5, 7, 1}, {6, 7, 13}, {19, 1, 10}}  //
  };

  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 0u), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, nm, 16u), ref_16);
}

TEST(Morpho, AlphaTreeCutMeanLabelization)
{
  mln::image2d<std::uint8_t> ima = {
      {1, 1, 5}, //
      {2, 5, 6}, //
      {0, 1, 4}  //
  };
  mln::image2d<std::uint8_t> cut_1 = {
      {1, 1, 5}, //
      {1, 5, 5}, //
      {0, 0, 4}  //
  };
  mln::image2d<std::uint8_t> cut_2 = {
      {1, 1, 5}, //
      {1, 5, 5}, //
      {1, 1, 5}  //
  };
  mln::image2d<std::uint8_t> cut_3 = {
      {2, 2, 2}, //
      {2, 2, 2}, //
      {2, 2, 2}  //
  };

  auto [t, nm] = mln::morpho::alphatree(ima, mln::c4);
  auto val     = t.compute_attribute_on_values(nm, ima, mln::accu::accumulators::mean<std::uint8_t, std::uint8_t>());

  // Renaming t and nm because clang does not allow to capture structured bindings
  auto make_cut = [&lt = t, &lnm = nm, &val](const typename decltype(t.values)::value_type threshold) {
    auto lbl = lt.horizontal_cut(threshold, lnm);
    return lt.reconstruct_from(lbl, ::ranges::make_span(val));
  };

  ASSERT_IMAGES_EQ_EXP(make_cut(0), ima);
  ASSERT_IMAGES_EQ_EXP(make_cut(1), cut_1);
  ASSERT_IMAGES_EQ_EXP(make_cut(2), cut_2);
  ASSERT_IMAGES_EQ_EXP(make_cut(3), cut_3);
}
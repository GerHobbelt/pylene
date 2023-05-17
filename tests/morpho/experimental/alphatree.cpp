#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/functional_ops.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/morpho/experimental/alphatree.hpp>
#include <mln/morpho/experimental/canvas/kruskal.hpp>

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

template <typename T>
void compare_vec_assert(std::vector<T> vec1, std::vector<T> vec2)
{
  EXPECT_EQ(vec1.size(), vec2.size());

  auto itvec1 = vec1.begin();
  auto itvec2 = vec2.begin();

  for (; itvec1 != vec1.end() && itvec2 != vec2.end(); itvec1++, itvec2++)
  {
    EXPECT_EQ(*itvec1, *itvec2);
  }
}

TEST(Morpho, AlphaTreeEdges)
{
  using I = mln::image2d<std::uint8_t>;
  using W = int;
  using E = mln::morpho::experimental::internal::edge<W>;

  mln::image2d<std::uint8_t>    image = {{4, 7, 3, 2}, {1, 2, 5, 3}, {1, 1, 5, 7}};
  mln::image_ch_value_t<I, int> map   = imchvalue<int>(image).set_init_value(-1);
  auto                          dom   = map.domain();
  int                           id    = 0;
  mln_foreach (auto p, dom)
  {
    map(p) = id;
    id++;
  }

  std::vector<E> ref = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                        {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                        {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<E> edges =
      mln::morpho::experimental::internal::make_edges(image, mln::c4, map, [](const auto& a, const auto& b) -> int {
        if (a > b)
          return a - b;
        else
          return b - a;
      });

  compare_vec_assert(ref, edges);
}

TEST(Morpho, AlphaTreeImageInt)
{
  const mln::image2d<int> image = {{4, 7, 3, 2}, {1, 2, 5, 3}, {1, 1, 5, 7}};

  auto [t, nm] =
      mln::morpho::experimental::alphatree(image, mln::c4, [](const auto& a, const auto& b) -> std::uint16_t {
        if (a > b)
          return a - b;
        else
          return b - a;
      });

  const mln::image2d<int>    refNm = {{6, 7, 8, 9}, {4, 10, 5, 11}, {4, 4, 5, 12}};
  std::vector<int>           refP  = {0, 0, 0, 1, 2, 1, 0, 0, 3, 3, 2, 3, 1};
  std::vector<std::uint16_t> refV  = {3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  compare_vec_assert(refP, t.parent);
  compare_vec_assert(refV, t.values);
  ASSERT_IMAGES_EQ_EXP(refNm, nm);
}

TEST(Morpho, AlphaTreeImageFloat)
{
  const mln::image2d<float> image = {{4.5, 7.1, 3.3, 2.8}, {1.8, 2.5, 5.4, 3.2}, {1.8, 1.9, 5.33, 7.25}};

  auto [t, nm] =
      mln::morpho::experimental::alphatree(image, mln::c4, [](const auto& a, const auto& b) -> float {
        if (a > b)
          return a - b;
        else
          return b - a;
      });

  const mln::image2d<int>    refNm = {{11, 12, 13, 14}, {10, 15, 16, 17}, {10, 18, 19, 20}};
  std::vector<int>           refP  = {0, 0, 1, 0, 3, 1, 3, 6, 5, 4, 8, 2, 2, 6, 7, 5, 9, 7, 8, 9, 4};
  std::vector<float> refV  = {2.9000001, 2.70000005, 2.5999999, 2.10000014, 1.92000008, 0.600000024, 0.5, 0.400000095, 0.100000024, 0.0700001717, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  compare_vec_assert(refP, t.parent);
  compare_vec_assert(refV, t.values);
  ASSERT_IMAGES_EQ_EXP(refNm, nm);
}

TEST(Morpho, AlphaTreeImage3d)
{
  const mln::image3d<std::uint8_t> ima = {{{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},
                                          {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}},
                                          {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}};

  const mln::image3d<int>   refNm = {{{16, 15, 15}, {16, 15, 15}, {17, 18, 19}},
                                     {{20, 21, 22}, {23, 24, 25}, {26, 14, 27}},
                                     {{28, 29, 30}, {14, 31, 32}, {33, 34, 35}}};
  std::vector<int>          refP  = {0, 0, 1, 2, 3,  3, 4, 4,  5, 7,  7, 9,  3, 5,  6,  8, 10, 10,
                                     3, 0, 9, 6, 11, 9, 8, 13, 1, 12, 9, 11, 8, 11, 12, 2, 8,  13};
  std::vector<std::uint8_t> refV  = {13, 12, 6, 5, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0,
                                     0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

  compare_vec_assert(refP, t.parent);
  compare_vec_assert(refV, t.values);
  ASSERT_IMAGES_EQ_EXP(refNm, nm);
}

TEST(Morpho, AlphaTreeCutImage2d)
{
  mln::image2d<int> ima = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };
  auto [t, node_map] = mln::morpho::experimental::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> int { return mln::functional::l2dist(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref_1 = {
      {0, 1, 2, 3, 4},      //
      {10, 6, 7, 8, 9},     //
      {10, 11, 12, 18, 14}, //
      {15, 16, 17, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };

  mln::image2d<int> ref_10 = {
      {6, 6, 7, 3, 4},      //
      {10, 6, 7, 8, 9},     //
      {10, 16, 12, 18, 14}, //
      {15, 16, 21, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };


  mln::image2d<int> ref_11 = {
      {6, 6, 7, 3, 9},      //
      {10, 6, 7, 8, 9},     //
      {10, 16, 12, 18, 14}, //
      {15, 16, 21, 18, 19}, //
      {15, 20, 21, 22, 23}, //
  };


  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 0), ref_0);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 1), ref_1);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 10), ref_10);
  ASSERT_IMAGES_EQ_EXP(cut(t, node_map, 11), ref_11);
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

  auto [t, _] = mln::morpho::experimental::alphatree(ima, mln::c4, [](const auto& a, const auto& b) -> int {
    if (a > b)
      return a - b;
    else
      return b - a;
  });

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

  auto [t, _] = mln::morpho::experimental::alphatree(ima, mln::c4, [](const auto& a, const auto& b) -> int {
    if (a > b)
      return a - b;
    else
      return b - a;
  });

  for (std::size_t i = 1; i < t.parent.size(); ++i)
    ASSERT_TRUE(t.values[i] != t.values[t.parent[i]]);
}

TEST(Morpho, AlphaTreeRGB8Uint16Distance)
{
  const mln::image2d<mln::rgb8> ima = {
      {{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},   //
      {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}}, //
      {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}    //
  };

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::uint16_t { return mln::functional::l2dist(a, b); });

  const mln::image2d<int> ref_0 = {
      {0, 0, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_8 = {
      {3, 3, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_16 = {
      {3, 3, 1}, //
      {5, 3, 4}, //
      {5, 6, 7}  //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::float_t { return mln::functional::l2dist(a, b); });

  const mln::image2d<int> ref_0 = {
      {0, 0, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_16 = {
      {3, 3, 1}, //
      {2, 3, 4}, //
      {5, 6, 7}  //
  };

  const mln::image2d<int> ref_32 = {
      {3, 3, 1}, //
      {5, 3, 4}, //
      {5, 6, 7}  //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c8, [](const auto& a, const auto& b) -> std::uint32_t { return mln::functional::l2dist(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 0, 6, 7, 8},      //
      {9, 10, 11, 12, 13},  //
      {14, 15, 16, 17, 18}, //
      {14, 19, 20, 21, 22}, //
  };

  mln::image2d<int> ref_16 = {
      {1, 1, 3, 3, 4},     //
      {5, 1, 6, 13, 12},   //
      {9, 15, 11, 12, 13}, //
      {9, 15, 17, 17, 21}, //
      {9, 19, 20, 21, 22}, //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c8, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

  mln::image2d<int> ref_0 = {
      {0, 1, 2, 3, 4},      //
      {5, 0, 6, 7, 8},      //
      {9, 10, 11, 12, 13},  //
      {14, 15, 16, 17, 18}, //
      {14, 19, 20, 21, 22}, //
  };

  mln::image2d<int> ref_16 = {
      {11, 1, 6, 3, 4},     //
      {9, 11, 6, 7, 8},     //
      {9, 15, 11, 17, 13},  //
      {14, 15, 20, 17, 21}, //
      {14, 19, 20, 21, 22}, //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint32_t { return mln::functional::l2dist(a, b); });

  const mln::image3d<int> ref_0 = {
      {{0, 1, 1}, {0, 1, 1}, {2, 3, 4}},         //
      {{5, 6, 7}, {8, 9, 10}, {11, 12, 13}},     //
      {{14, 15, 16}, {12, 17, 18}, {19, 20, 21}} //
  };

  const mln::image3d<int> ref_16 = {
      {{0, 2, 2}, {0, 2, 2}, {2, 3, 4}},        //
      {{14, 6, 7}, {8, 20, 21}, {11, 0, 18}},   //
      {{14, 17, 20}, {0, 17, 18}, {19, 20, 21}} //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

  const mln::image3d<int> ref_0 = {
      {{0, 1, 1}, {0, 1, 1}, {2, 3, 4}},         //
      {{5, 6, 7}, {8, 9, 10}, {11, 12, 13}},     //
      {{14, 15, 16}, {12, 17, 18}, {19, 20, 21}} //
  };

  const mln::image3d<int> ref_16 = {
      {{2, 20, 20}, {2, 20, 20}, {2, 3, 4}},    //
      {{14, 6, 17}, {14, 20, 21}, {11, 6, 18}}, //
      {{14, 17, 20}, {6, 17, 18}, {19, 20, 21}} //
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

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c4, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });
  auto val = t.compute_attribute_on_values(nm, ima, mln::accu::accumulators::mean<std::uint8_t, std::uint8_t>());

  // Renaming t and nm because clang does not allow to capture structured bindings
  auto make_cut = [&lt = t, &lnm = nm, &val](auto threshold) {
    auto lbl = lt.horizontal_cut(threshold, lnm);
    return lt.reconstruct_from(lbl, ::ranges::make_span(val));
  };

  ASSERT_IMAGES_EQ_EXP(make_cut(0), ima);
  ASSERT_IMAGES_EQ_EXP(make_cut(1), cut_1);
  ASSERT_IMAGES_EQ_EXP(make_cut(2), cut_2);
  ASSERT_IMAGES_EQ_EXP(make_cut(3), cut_3);
}
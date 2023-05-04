#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/morpho/alphatree.hpp>
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
void ASSERT_VEC_EQ(T const& ref, T const& vec)
{
  EXPECT_EQ(ref.size(), vec.size());

  auto itref = ref.begin();
  auto itvec = vec.begin();

  for (; itref != ref.end() && itvec != vec.end(); itref++, itvec++)
  {
    EXPECT_EQ(*itref, *itvec);
  }
}

TEST(AlphaTree, edges)
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

  ASSERT_VEC_EQ(ref, edges);
}

TEST(AlphaTree, imageInt)
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

  ASSERT_VEC_EQ(refP, t.parent);
  ASSERT_VEC_EQ(refV, t.values);
  ASSERT_IMAGES_EQ_EXP(refNm, nm);
}

/*TEST(AlphaTree, Image3d)
{
  const mln::image3d<std::uint8_t> ima = {{{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},
                                     {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}},
                                     {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}};

  const mln::image3d<std::uint8_t> refNm = {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}},
                                            {{9, 10, 11}, {12, 13, 14}, {15, 16, 17}},
                                            {{18, 19, 20}, {21, 22, 23}, {24, 25, 26}}};
  std::vector<int>                 refP  = {27, 30, 30, 27, 30, 30, 37, 49, 52, 40, 44, 35, 40, 42, 33, 51, 31, 34,
                                            40, 35, 42, 31, 35, 34, 50, 42, 33, 37, 28, 29, 42, 44, 32, 45, 49, 40,
                                            36, 43, 38, 39, 43, 41, 45, 46, 46, 49, 49, 47, 48, 50, 51, 52, -1};
  std::vector<std::uint8_t> refV = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,
                               0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 2, 0, 0, 2, 0, 2, 3, 3, 3, 4, 0, 0, 5, 6, 12, 13};

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

  ASSERT_VEC_EQ(refP, t.parent);
  ASSERT_VEC_EQ(refV, t.values);
  ASSERT_IMAGES_EQ_EXP(refNm, nm);
}*/

TEST(AlphaTree, cutImage2d)
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
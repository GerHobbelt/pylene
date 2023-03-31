#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/experimental/alphatree.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

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

TEST(AlphaTree, kruskalBt)
{
  using E = mln::morpho::experimental::internal::edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                    = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::kruskal(q, edges, 12);

  ASSERT_VEC_EQ(refP, q.parent);
  ASSERT_VEC_EQ(refMST, MST);
}

TEST(AlphaTree, Qt)
{
  using E = mln::morpho::experimental::internal::edge<std::int16_t>;


  std::vector<E>                           MST = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                  {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QBT q   = {12};
  q.parent             = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  q.size               = 23;
  std::vector<int> ref = {22, 22, 16, 16, 14, 17, 13, 16, 14, 14, 13, 19, 12, 19, 17, 15, 19, 22, 18, 22, 20, 21, -1};
  std::vector<int> res = mln::morpho::experimental::internal::canonize_qbt(q, MST);

  ASSERT_VEC_EQ(ref, res);
}

TEST(AlphaTree, kruskalEbt)
{
  using E = mln::morpho::experimental::internal::edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                     = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                 {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QEBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::kruskal(q, edges, 12);

  ASSERT_VEC_EQ(refP, q.qbt.parent);
  ASSERT_VEC_EQ(refMST, MST);
}

TEST(AlphaTree, full_int)
{
  const mln::image2d<std::uint8_t> image = {{4, 7, 3, 2}, {1, 2, 5, 3}, {1, 1, 5, 7}};

  auto [t, nm] =
      mln::morpho::experimental::alphatree(image, mln::c4, [](const auto& a, const auto& b) -> std::uint16_t {
        if (a > b)
          return a - b;
        else
          return b - a;
      });

  const mln::image2d<std::uint8_t> refNm = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};
  std::vector<int> refP = {22, 22, 16, 16, 14, 17, 13, 16, 14, 14, 13, 19, 12, 19, 17, 15, 19, 22, 18, 22, 20, 21, -1};
  std::vector<uint16_t> refV = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2, 0, 0, 3};

  mln_foreach (auto p, nm.domain())
  {
    ASSERT_EQ(nm(p), refNm(p));
  }

  ASSERT_VEC_EQ(refP, t.parent);
  ASSERT_VEC_EQ(refV, t.values);
}

TEST(AlphaTree, Image3d)
{
  const mln::image3d<uint8_t> ima = {{{10, 0, 0}, {10, 0, 0}, {12, 20, 38}},
                                     {{13, 22, 16}, {15, 2, 6}, {37, 25, 12}},
                                     {{11, 18, 0}, {25, 17, 11}, {9, 0, 5}}};

  const mln::image3d<std::uint8_t> refNm = {{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}},
                                            {{9, 10, 11}, {12, 13, 14}, {15, 16, 17}},
                                            {{18, 19, 20}, {21, 22, 23}, {24, 25, 26}}};
  std::vector<int>                 refP  = {27, 30, 30, 27, 30, 30, 37, 49, 52, 40, 44, 35, 40, 42, 33, 51, 31, 34,
                                            40, 35, 42, 31, 35, 34, 50, 42, 33, 37, 28, 29, 42, 44, 32, 45, 49, 40,
                                            36, 43, 38, 39, 43, 41, 45, 46, 46, 49, 49, 47, 48, 50, 51, 52, -1};
  std::vector<uint8_t> refV = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,
                               0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 2, 0, 0, 2, 0, 2, 3, 3, 3, 4, 0, 0, 5, 6, 12, 13};

  auto [t, nm] = mln::morpho::experimental::alphatree(
      ima, mln::c26, [](const auto& a, const auto& b) -> std::uint8_t { return mln::functional::l2dist(a, b); });

  mln_foreach (auto p, nm.domain())
  {
    ASSERT_EQ(nm(p), refNm(p));
  }

  ASSERT_VEC_EQ(refP, t.parent);
  ASSERT_VEC_EQ(refV, t.values);
}
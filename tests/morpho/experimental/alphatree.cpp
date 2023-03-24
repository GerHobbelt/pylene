#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
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

TEST(AlphaTree, adjList)
{
  using I = mln::image2d<std::uint8_t>;
  using P = mln::image_point_t<I>;
  using W = std::int16_t;
  using E = mln::morpho::experimental::internal::edge<W>;

  std::map<P, int> map = {};

  mln::image2d<std::uint8_t> image = {{4, 7, 3, 2}, {1, 2, 5, 3}, {1, 1, 5, 7}};
  auto                       dom   = image.domain();
  int                        id    = 0;
  mln_foreach (auto p, dom)
  {
    map.insert({p, id});
    id++;
  }

  std::vector<E> ref = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                        {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                        {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<E> adjList = {};
  mln::morpho::experimental::internal::MakeAdjList(image, mln::c4, map, adjList);

  ASSERT_VEC_EQ(ref, adjList);
}

TEST(AlphaTree, kruskalBt)
{
  using E = mln::morpho::experimental::internal::edge<std::int16_t>;

  std::vector<E> adjList = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                            {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                            {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                    = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::Kruskal(q, adjList, 12);

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
  std::vector<int> ref = {21, 21, 16, 16, 14, 17, 13, 16, 14, 14, 13, 19, 12, 18, 17, 15, 18, 21, 19, 22, 20, 22, -1};
  std::vector<int> res = mln::morpho::experimental::internal::CanonizeQbt(q, MST);

  ASSERT_VEC_EQ(ref, res);
}

TEST(AlphaTree, kruskalEbt)
{
  using E = mln::morpho::experimental::internal::edge<std::int16_t>;

  std::vector<E> adjList = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                            {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                            {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                     = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                 {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QEBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::Kruskal(q, adjList, 12);

  ASSERT_VEC_EQ(refP, q.qbt.parent);
  ASSERT_VEC_EQ(refMST, MST);

  return;
}

TEST(AlphaTree, full_uint8)
{
  using I = mln::image2d<std::uint8_t>;
  using P = mln::image_point_t<I>;
  using W = std::uint8_t;
  using E = mln::morpho::experimental::internal::edge<W>;

  std::map<P, int> map = {};

  mln::image2d<std::uint8_t> image = {{4, 7, 3, 2}, {1, 2, 5, 3}, {1, 1, 5, 7}};
  auto                       dom   = image.domain();
  int                        id    = 0;
  mln_foreach (auto p, dom)
  {
    map.insert({p, id});
    id++;
  }

  std::vector<E> ref = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                        {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                        {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<E> adjList = {};
  mln::morpho::experimental::internal::MakeAdjList(image, mln::c4, map, adjList);

  ASSERT_VEC_EQ(ref, adjList);

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                     = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                 {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QEBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::Kruskal(q, adjList, 12);

  ASSERT_VEC_EQ(refP, q.qbt.parent);
  ASSERT_VEC_EQ(refMST, MST);
}

TEST(AlphaTree, full_char)
{
  using I = mln::image2d<unsigned char>;
  using P = mln::image_point_t<I>;
  using W = unsigned char;
  using E = mln::morpho::experimental::internal::edge<W>;

  std::map<P, int> map = {};

  mln::image2d<unsigned char> image = {{'d', 'g', 'c', 'b'}, {'a', 'b', 'e', 'c'}, {'a', 'a', 'e', 'g'}};
  auto                        dom   = image.domain();
  int                         id    = 0;
  mln_foreach (auto p, dom)
  {
    map.insert({p, id});
    id++;
  }

  std::vector<E> ref = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                        {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                        {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<E> adjList = {};
  mln::morpho::experimental::internal::MakeAdjList(image, mln::c4, map, adjList);

  ASSERT_VEC_EQ(ref, adjList);

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST                     = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                                                 {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::internal::QEBT q = {12};

  std::vector<E> MST = mln::morpho::experimental::internal::Kruskal(q, adjList, 12);

  ASSERT_VEC_EQ(refP, q.qbt.parent);
  ASSERT_VEC_EQ(refMST, MST);
}
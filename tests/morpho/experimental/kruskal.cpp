#include <mln/accu/accumulators/mean.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/experimental/canvas/kruskal.hpp>

#include <gtest/gtest.h>

template <typename W>
class edge
{
public:
  int a;
  int b;
  W   w;

  edge(int a, int b, W w)
    : a(a)
    , b(b)
    , w(w)
  {
  }

  bool operator==(edge<W> o) const { return a == o.a && b == o.b && w == o.w; }
};

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

TEST(AlphaTree, KruskalVisitorBase)
{
  using E = edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  mln::morpho::experimental::canvas::kruskal_visitor_base v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
}

TEST(AlphaTree, KruskalVisitorMst)
{
  using E = edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<E>   refMST = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                             {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::canvas::kruskal_visitor_mst<E> v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
  ASSERT_VEC_EQ(refMST, v.mst);
}

TEST(AlphaTree, KruskalVisitorValues)
{
  using E = edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};
  std::vector<int> refV = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3};

  mln::morpho::experimental::canvas::kruskal_visitor_values<E> v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
  ASSERT_VEC_EQ(refV, v.values);
}

TEST(AlphaTree, KruskalVisitorNodemap)
{
  using E = edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {20, 20, 15, 15, 12, 17, 13, 16, 12, 14, 13, 19, 14, 18, 17, 16, 18, 21, 19, 22, 21, 22, -1};

  mln::morpho::experimental::canvas::kruskal_visitor_nodemap v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
}
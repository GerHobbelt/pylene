#include <mln/accu/accumulators/mean.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/experimental/canvas/kruskal.hpp>

#include <gtest/gtest.h>
// deplacer dans kruskal
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
// FIXME code pylene
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

TEST(Kruskal, KruskalVisitorBase)
{
  using E = edge<std::int16_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};


  std::vector<int> refP = {-1, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  // const mln::image2d<std::uint8_t> refNm = {{11, 12, 13, 14}, {15, 16, 17, 18}, {19, 20, 21, 22}};

  mln::morpho::experimental::canvas::kruskal_visitor_base v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  /*mln_foreach (auto p, v.nodemap.domain())
  {
    EXPECT_EQ(v.nodemap(p), refNm(p));
  }*/

  ASSERT_VEC_EQ(refP, v.parent);
}

TEST(Kruskal, KruskalVisitorMst)
{
  using E = edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};


  std::vector<int> refP   = {-1, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  std::vector<E>   refMST = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                             {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};
  mln::morpho::experimental::canvas::kruskal_visitor_mst<I, E> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
  ASSERT_VEC_EQ(refMST, v.mst);
}

TEST(Kruskal, KruskalVisitorValues)
{
  using E = edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};

  std::vector<int> refP = {-1, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  std::vector<std::int16_t> refV = {3, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  mln::morpho::experimental::canvas::kruskal_visitor_values<I, std::int16_t> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
  ASSERT_VEC_EQ(refV, v.value);
}

TEST(Kruskal, KruskalVisitorCanonized)
{
  using E = edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};

  std::vector<int> refP = {-1, 0, 0, 1, 2, 1, 0, 0, 3, 3, 4, 2, 5, 3, 4, 4, 5, 1};

  mln::morpho::experimental::canvas::kruskal_visitor_canonized<I, std::int16_t> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  ASSERT_VEC_EQ(refP, v.parent);
}
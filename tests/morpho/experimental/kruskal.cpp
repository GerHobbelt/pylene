#include <mln/accu/accumulators/mean.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/experimental/canvas/kruskal.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>

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

template <typename T>
void compare_vec_assert_cast(std::vector<T> vec1, std::vector<std::any> vec2)
{
  EXPECT_EQ(vec1.size(), vec2.size());

  auto itvec1 = vec1.begin();
  auto itvec2 = vec2.begin();

  for (; itvec1 != vec1.end() && itvec2 != vec2.end(); itvec1++, itvec2++)
  {
    try
    {
      EXPECT_EQ(*itvec1, std::any_cast<T>(*itvec2));
    }
    catch (const std::bad_any_cast& e)
    {
      EXPECT_EQ(*itvec1, std::any_cast<int>(*itvec2));
    }
  }
}

TEST(Kruskal, KruskalVisitorBase)
{
  using E = mln::morpho::experimental::canvas::internal::edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP = {0, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};

  mln::morpho::experimental::canvas::kruskal_visitor_base v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  compare_vec_assert(refP, v.parent);
}

TEST(Kruskal, KruskalVisitorMst)
{
  using E = mln::morpho::experimental::canvas::internal::edge<std::int16_t>;

  std::vector<E> edges = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                          {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                          {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};

  std::vector<int> refP   = {0, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  std::vector<E>   refMST = {{4, 8, 0}, {6, 10, 0},  {8, 9, 0}, {2, 3, 1}, {3, 7, 1}, {4, 5, 1},
                             {2, 6, 2}, {10, 11, 2}, {0, 1, 3}, {0, 4, 3}, {5, 6, 3}};

  mln::morpho::experimental::canvas::kruskal_visitor_mst<std::int16_t> v = {12};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  compare_vec_assert(refP, v.parent);
  compare_vec_assert(refMST, v.mst);
}

TEST(Kruskal, KruskalVisitorValues)
{
  using E = mln::morpho::experimental::canvas::internal::edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};

  std::vector<int>                 refP  = {0, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  std::vector<std::int16_t>        refV  = {3, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const mln::image2d<std::uint8_t> refNm = {{2, 2, 7, 7}, {10, 5, 9, 6}, {10, 8, 9, 3}};

  mln::morpho::experimental::canvas::kruskal_visitor_values<I, std::int16_t> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  compare_vec_assert(refP, v.parent);
  compare_vec_assert(refV, v.value);
  ASSERT_IMAGES_EQ_EXP(refNm, v.nodemap);
}

TEST(Kruskal, KruskalVisitorCanonized)
{
  using E = mln::morpho::experimental::canvas::internal::edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};

  std::vector<int>                 refP  = {0, 0, 0, 1, 2, 1, 0, 0, 3, 3, 2, 3, 1};
  std::vector<std::int16_t>        refV  = {3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const mln::image2d<std::uint8_t> refNm = {{6, 7, 8, 9}, {4, 10, 5, 11}, {4, 4, 5, 12}};

  mln::morpho::experimental::canvas::kruskal_visitor_canonized<I, std::int16_t> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  compare_vec_assert(refP, v.parent);
  compare_vec_assert(refV, v.value);
  ASSERT_IMAGES_EQ_EXP(refNm, v.nodemap);
}

TEST(Kruskal, KruskalVisitorValueErased)
{
  using E = mln::morpho::experimental::canvas::internal::edge<std::int16_t>;
  using I = mln::image2d<std::uint8_t>;

  std::vector<E>                   edges   = {{0, 1, 3},  {0, 4, 3},  {1, 2, 4}, {1, 5, 5},  {2, 3, 1},  {2, 6, 2},
                                              {3, 7, 1},  {4, 5, 1},  {4, 8, 0}, {5, 6, 3},  {5, 9, 1},  {6, 7, 2},
                                              {6, 10, 0}, {7, 11, 4}, {8, 9, 0}, {9, 10, 4}, {10, 11, 2}};
  const mln::image2d<std::uint8_t> nodemap = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};

  std::vector<int>                 refP  = {0, 0, 1, 0, 3, 1, 4, 6, 5, 4, 8, 2, 2, 7, 7, 10, 5, 9, 6, 10, 8, 9, 3};
  std::vector<std::int16_t>        refV  = {3, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const mln::image2d<std::uint8_t> refNm = {{2, 2, 7, 7}, {10, 5, 9, 6}, {10, 8, 9, 3}};

  mln::morpho::experimental::canvas::kruskal_visitor_values_erased<I> v = {12, nodemap};

  mln::morpho::experimental::canvas::kruskal(v, edges);

  compare_vec_assert(refP, v.parent);
  compare_vec_assert_cast(refV, v.value);
  ASSERT_IMAGES_EQ_EXP(refNm, v.nodemap);
}
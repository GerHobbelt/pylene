#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>

#include <mln/morpho/tos/private/immersion.hpp>
#include <mln/morpho/tos/private/propagation.hpp>
#include <mln/morpho/tos/private/propagation.hpp>
#include <mln/morpho/tos/v2/tos.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/accu/accumulators/max.hpp>

#include <gtest/gtest.h>
#include <tests/helpers.hpp>
#include <algorithm>
#include "tos_tests_helper.hpp"

using mln::uint8;



template <class I>
void test_propagation(const mln::Image<I>& f, const mln::image2d<uint8>& ref)
{
  using P = mln::image2d<uint8>::size_type;
  std::vector<P> indexes;
  int max_depth;
  auto g = mln::morpho::tos::impl::immersion(f);
  auto ord = mln::morpho::tos::impl::propagation(g, {0,0}, max_depth, &indexes);

  int expected_max_depth = mln::accumulate(ref, mln::accu::features::max<>());

  ASSERT_IMAGES_EQ(ord, ref);
  ASSERT_EQ(expected_max_depth, max_depth);
  ASSERT_TRUE(std::is_sorted(indexes.begin(), indexes.end(), [&ord](P i, P j) { return ord[i] < ord[j]; }));
}


template <class I>
void test_construction(const mln::Image<I>& f,
                       const mln::image2d<mln::point2d>& ref_parent,
                       const mln::image2d<mln::point2d>& ref_roots)
{
  auto tree = mln::morpho::__2::tos(f);

  mln::image2d<mln::point2d> roots;
  auto par = tree_as_parent_image(tree, roots);

  ASSERT_IMAGES_EQ(par, ref_parent);
  ASSERT_IMAGES_EQ(roots, ref_roots);
}

template <class T>
class ToSPropagation : public ::testing::Test
{
};

typedef ::testing::Types<mln::uint8, float> TestValueTypes;
TYPED_TEST_CASE(ToSPropagation, TestValueTypes);

TYPED_TEST(ToSPropagation, saddle_point)
{
  const mln::image2d<TypeParam> f = { {0, 0, 0, 0},
                                      {0, 2, 0, 0},
                                      {0, 0, 2, 0},
                                      {0, 0, 0, 0} };

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 1, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 1, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0}};
  test_propagation(f, ref);
}

TYPED_TEST(ToSPropagation, two_branches_with_parallel_flooding)
{
  const mln::image2d<TypeParam> f = { {0, 0, 0, 0},
                                  {0, 2, 3, 0},
                                  {0, 0, 2, 0},
                                  {1, 2, 0, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 2, 2, 3, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {1, 1, 2, 0, 0, 0, 0}};
  test_propagation(f, ref);
}

TYPED_TEST(ToSPropagation, maxbounds)
{
  const mln::image2d<TypeParam> f = { {0,   255, 0,   255},
                                      {0,   255, 0,   0  },
                                      {255, 255, 255, 255},
                                      {0,   255, 255, 0  }};

  const mln::image2d<uint8> ref = {{0, 0, 1, 1, 2, 2, 3},
                                   {0, 0, 1, 1, 2, 2, 2},
                                   {0, 0, 1, 1, 2, 2, 2},
                                   {0, 0, 1, 1, 1, 1, 1},
                                   {1, 1, 1, 1, 1, 1, 1},
                                   {1, 1, 1, 1, 1, 1, 1},
                                   {2, 1, 1, 1, 1, 1, 2}};
  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, chessboard)
{
  const mln::image2d<TypeParam> f = { {0,   255,   0, 255},
                                      {255,   0, 255,   0},
                                      {0,   255,   0, 255},
                                      {255,   0, 255,   0} };


  const mln::image2d<uint8> ref = {{0, 0, 1, 0, 0, 0, 1},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {1, 0, 0, 0, 1, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 1, 0, 0, 0, 1},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {1, 0, 0, 0, 1, 0, 0}};

  test_propagation(f, ref);
}

TYPED_TEST(ToSPropagation, input_has_no_border)
{
  std::initializer_list<std::initializer_list<TypeParam>> data = {{0, 0, 0, 0},
                                                              {0, 2, 3, 0},
                                                              {0, 0, 2, 0},
                                                              {1, 2, 0, 0}};
  const mln::image2d<TypeParam> f(data, 0);

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 2, 2, 3, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {1, 1, 2, 0, 0, 0, 0}};

  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, input_is_a_view)
{
  const mln::image2d<TypeParam> f = { {0, 0, 0, 0},
                                      {0, 2, 3, 0},
                                      {0, 0, 2, 0},
                                      {1, 2, 0, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {0, 0, 2, 2, 3, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 2, 0, 0},
                                   {0, 0, 0, 0, 0, 0, 0},
                                   {1, 1, 2, 0, 0, 0, 0}};
  test_propagation(mln::imcast<TypeParam>(2 * f), ref);
}


TEST(ToSConstruction, saddle_point)
{
  const mln::image2d<uint8> f = { {0, 0, 0, 0},
                                  {0, 2, 0, 0},
                                  {0, 0, 2, 0},
                                  {0, 0, 0, 0} };

  const mln::point2d a = {0, 0};
  const mln::point2d b = {2, 2};
  const mln::point2d c = {4, 4};
  const mln::image2d<mln::point2d> ref_roots = {{a, a, a, a, a, a, a},
                                                {a, a, a, a, a, a, a},
                                                {a, a, b, a, a, a, a},
                                                {a, a, a, a, a, a, a},
                                                {a, a, a, a, c, a, a},
                                                {a, a, a, a, a, a, a},
                                                {a, a, a, a, a, a, a}};

  const mln::image2d<mln::point2d> ref_parent = {{a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a}};
  test_construction(f, ref_parent, ref_roots);
}


TEST(ToSConstruction, two_branches_with_parallel_flooding)
{
  const mln::image2d<uint8> f = { {0, 0, 0, 0},
                                  {0, 2, 3, 0},
                                  {0, 0, 2, 0},
                                  {1, 2, 0, 0}};


  const mln::point2d a = {0, 0};
  const mln::point2d b = {2, 2};
  const mln::point2d c = {2, 4};
  const mln::point2d d = {6, 0};
  const mln::point2d e = {6, 2};

  const mln::image2d<mln::point2d> ref_parent = {{a, a, a, a, a, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, a, a, b, b, a, a},
                                                 {a, a, a, a, b, a, a},
                                                 {a, a, a, a, b, a, a},
                                                 {a, a, a, a, a, a, a},
                                                 {a, d, d, a, a, a, a}};

  const mln::image2d<mln::point2d> ref_roots = {{a, a, a, a, a, a, a},
                                                {a, a, a, a, a, a, a},
                                                {a, a, b, b, c, a, a},
                                                {a, a, a, a, b, a, a},
                                                {a, a, a, a, b, a, a},
                                                {a, a, a, a, a, a, a},
                                                {d, d, e, a, a, a, a}};

  test_construction(f, ref_parent, ref_roots);
}

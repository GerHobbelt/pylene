#include <mln/morpho/experimental/private/immersion.hpp>
#include <mln/morpho/experimental/private/propagation.hpp>
#include <mln/morpho/experimental/tos.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/accu/accumulators/max.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>

#include "tos_tests_helper.hpp"


TEST(ToSImmersion, twodimensional)
{
  const mln::experimental::image2d<uint8_t> f = {{0, 1, 2}, //
                                                 {3, 0, 1}};


  const mln::experimental::image2d<uint8_t> ref_inf = {{0, 0, 1, 1, 2}, //
                                                       {0, 0, 0, 0, 1}, //
                                                       {3, 0, 0, 0, 1}};

  const mln::experimental::image2d<uint8_t> ref_sup = {{0, 1, 1, 2, 2}, //
                                                       {3, 3, 1, 2, 2}, //
                                                       {3, 3, 0, 1, 1}};


  auto [inf, sup] = mln::morpho::experimental::details::immersion(f);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}

TEST(ToSImmersion, twodimensional_generic)
{
  using namespace mln::view::ops;
  const mln::experimental::image2d<uint8_t> f = {{0, 1, 2}, //
                                                 {3, 0, 1}};

  auto fprime = f * uint8_t(1);


  const mln::experimental::image2d<uint8_t> ref_inf = {{0, 0, 1, 1, 2}, //
                                                       {0, 0, 0, 0, 1}, //
                                                       {3, 0, 0, 0, 1}};

  const mln::experimental::image2d<uint8_t> ref_sup = {{0, 1, 1, 2, 2}, //
                                                       {3, 3, 1, 2, 2}, //
                                                       {3, 3, 0, 1, 1}};


  auto [inf, sup] = mln::morpho::experimental::details::immersion(fprime);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}

TEST(ToSImmersion, threedimensional)
{
  const mln::experimental::image3d<uint8_t> f = {
      {{0, 1, 2}, {3, 0, 1}}, //
      {{2, 0, 4}, {0, 0, 2}},
  };


  const mln::experimental::image3d<uint8_t> ref_inf = {
      {{0, 0, 1, 1, 2},  //
       {0, 0, 0, 0, 1},  //
       {3, 0, 0, 0, 1}}, //
      {{0, 0, 0, 0, 2},  //
       {0, 0, 0, 0, 1},  //
       {0, 0, 0, 0, 1}}, //
      {{2, 0, 0, 0, 4},  //
       {0, 0, 0, 0, 2},  //
       {0, 0, 0, 0, 2}}, //
  };

  const mln::experimental::image3d<uint8_t> ref_sup = {
      {{0, 1, 1, 2, 2},  //
       {3, 3, 1, 2, 2},  //
       {3, 3, 0, 1, 1}}, //
      {{2, 2, 1, 4, 4},  //
       {3, 3, 1, 4, 4},  //
       {3, 3, 0, 2, 2}}, //
      {{2, 2, 0, 4, 4},  //
       {2, 2, 0, 4, 4},  //
       {0, 0, 0, 2, 2}}, //
  };

  auto [inf, sup] = mln::morpho::experimental::details::immersion(f);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}


template <class I, class J>
void test_propagation(I& f, J& ref)
{
  //std::vector<P> indexes;

  int max_depth;
  auto [inf, sup] = mln::morpho::experimental::details::immersion(f);

  mln::image_ch_value_t<I, int> ord(inf.domain());

  mln::morpho::experimental::details::propagation(inf, sup, ord, inf.domain().tl(), max_depth);

  int  expected_max_depth = mln::accumulate(ref, mln::accu::features::max<>());
  ASSERT_IMAGES_EQ_EXP(ord, ref);
  ASSERT_EQ(expected_max_depth, max_depth);
  //ASSERT_TRUE(std::is_sorted(indexes.begin(), indexes.end(), [&ord](P i, P j) { return ord[i] < ord[j]; }));
}

template <class T>
class ToSPropagation : public ::testing::Test
{
};

typedef ::testing::Types<mln::uint8> TestValueTypes;
TYPED_TEST_CASE(ToSPropagation, TestValueTypes);

TYPED_TEST(ToSPropagation, saddle_point)
{
  const mln::experimental::image2d<TypeParam> f = {{0, 0, 0, 0}, //
                                                   {0, 2, 0, 0}, //
                                                   {0, 0, 2, 0}, //
                                                   {0, 0, 0, 0}};

  const mln::experimental::image2d<int> ref = {{0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 1, 0, 0, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 0, 0, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}};
  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, two_branches_with_parallel_flooding)
{
  const mln::experimental::image2d<TypeParam> f = {{0, 0, 0, 0}, //
                                                   {0, 2, 3, 0}, //
                                                   {0, 0, 2, 0}, //
                                                   {1, 2, 0, 0}};

  const mln::experimental::image2d<int> ref = {{0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 2, 2, 3, 0, 0}, //
                                               {0, 0, 0, 0, 2, 0, 0}, //
                                               {0, 0, 0, 0, 2, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {1, 1, 2, 0, 0, 0, 0}};
  test_propagation(f, ref);
}

TYPED_TEST(ToSPropagation, two_branches_with_parallel_flooding_3d)
{
  const mln::experimental::image3d<TypeParam> f = {{{0, 0, 0},  //
                                                    {0, 2, 3},  //
                                                    {0, 0, 2}}, //
                                                   {{1, 0, 1},  //
                                                    {0, 2, 1},  //
                                                    {1, 0, 2}}};


  const mln::experimental::image3d<int> ref = {{{0, 0, 0, 0, 0},  //
                                                  {0, 0, 0, 0, 0},  //
                                                  {0, 0, 2, 2, 3},  //
                                                  {0, 0, 0, 0, 2},  //
                                                  {0, 0, 0, 0, 2}}, //
                                                 {{0, 0, 0, 0, 0},  //
                                                  {0, 0, 0, 0, 0},  //
                                                  {0, 0, 2, 1, 1},  //
                                                  {0, 0, 0, 0, 1},  //
                                                  {0, 0, 0, 0, 2}}, //
                                                 {{1, 0, 0, 0, 1},  //
                                                  {0, 0, 0, 0, 1},  //
                                                  {0, 0, 2, 1, 1},  //
                                                  {0, 0, 0, 0, 1},  //
                                                  {1, 0, 0, 0, 2}}};
  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, maxbounds)
{
  const mln::experimental::image2d<TypeParam> f = {{0, 255, 0, 255},     //
                                                   {0, 255, 0, 0},       //
                                                   {255, 255, 255, 255}, //
                                                   {0, 255, 255, 0}};

  const mln::experimental::image2d<int> ref = {{0, 0, 1, 1, 2, 2, 3}, //
                                               {0, 0, 1, 1, 2, 2, 2}, //
                                               {0, 0, 1, 1, 2, 2, 2}, //
                                               {0, 0, 1, 1, 1, 1, 1}, //
                                               {1, 1, 1, 1, 1, 1, 1}, //
                                               {1, 1, 1, 1, 1, 1, 1}, //
                                               {2, 1, 1, 1, 1, 1, 2}};
  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, chessboard)
{
  const mln::experimental::image2d<TypeParam> f = {{0, 255, 0, 255}, //
                                                   {255, 0, 255, 0}, //
                                                   {0, 255, 0, 255}, //
                                                   {255, 0, 255, 0}};


  const mln::experimental::image2d<int> ref = {{0, 0, 1, 0, 0, 0, 1}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {1, 0, 0, 0, 1, 0, 0}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {0, 0, 1, 0, 0, 0, 1}, //
                                               {0, 0, 0, 0, 0, 0, 0}, //
                                               {1, 0, 0, 0, 1, 0, 0}};

  test_propagation(f, ref);
}

TEST(ToSConstruction, saddle_point)
{
  const mln::experimental::image2d<uint8_t> f = {{0, 0, 0, 0}, //
                                                 {0, 2, 0, 0}, //
                                                 {0, 0, 2, 0}, //
                                                 {0, 0, 0, 0}};

  const mln::experimental::point2d                            a         = {0, 0};
  const mln::experimental::point2d                            b         = {2, 2};
  const mln::experimental::point2d                            c         = {4, 4};
  const mln::experimental::image2d<mln::experimental::point2d> ref_roots = {{a, a, a, a, a, a, a}, //
                                                                            {a, a, a, a, a, a, a}, //
                                                                            {a, a, b, a, a, a, a}, //
                                                                            {a, a, a, a, a, a, a}, //
                                                                            {a, a, a, a, c, a, a}, //
                                                                            {a, a, a, a, a, a, a}, //
                                                                            {a, a, a, a, a, a, a}};

  const mln::experimental::image2d<mln::experimental::point2d> ref_parent = {{a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}, //
                                                                             {a, a, a, a, a, a, a}};
  auto [tree, node_map ] = mln::morpho::experimental::tos(f, {0,0});
  compare_tree_to_ref(tree, node_map, ref_parent, ref_roots);
}

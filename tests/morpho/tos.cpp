#include <mln/core/image/image2d.hpp>
#include <mln/core/image/image3d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>

#include <mln/accu/accumulators/max.hpp>
#include <mln/core/algorithm/accumulate.hpp>
#include <mln/morpho/tos/private/immersion.hpp>
#include <mln/morpho/tos/private/propagation.hpp>
#include <mln/morpho/tos/tos.hpp>

#include "tos_tests_helper.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <helpers.hpp>

using mln::uint8;


template <class I, class J>
void test_propagation(const mln::Image<I>& f, const mln::Image<J>& ref)
{
  using P = typename I::size_type;
  std::vector<P> indexes;
  int            max_depth;
  auto           g  = mln::morpho::ToS::impl::immersion(f);
  mln_point(I) pmin = 0;
  auto ord          = mln::morpho::ToS::impl::propagation(g, pmin, max_depth, &indexes);

  int expected_max_depth = mln::accumulate(ref, mln::accu::features::max<>());

  ASSERT_IMAGES_EQ(ord, ref);
  ASSERT_EQ(expected_max_depth, max_depth);
  ASSERT_TRUE(std::is_sorted(indexes.begin(), indexes.end(), [&ord](P i, P j) { return ord[i] < ord[j]; }));
}


template <class I, class J>
void test_construction(const mln::Image<I>& f, const mln::Image<J>& ref_parent, const mln::Image<J>& ref_roots)
{
  auto tree = mln::morpho::tos(f);

  mln_ch_value(J, mln_point(J)) roots;
  auto par = tree_as_parent_image(tree, roots);

  ASSERT_IMAGES_EQ(par, ref_parent);
  ASSERT_IMAGES_EQ(roots, ref_roots);
}

TEST(ToSImmersion, twodimensional)
{
  const mln::image2d<mln::uint8> f = {{0, 1, 2}, {3, 0, 1}};

  using R = mln::morpho::ToS::impl::irange<mln::uint8>;

  const mln::image2d<R> ref = {{{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}},
                               {{0, 3}, {0, 3}, {0, 1}, {0, 2}, {1, 2}},
                               {{3, 3}, {0, 3}, {0, 0}, {0, 1}, {1, 1}}};

  auto g = mln::morpho::ToS::impl::immersion(f);
  ASSERT_IMAGES_EQ(ref, g);
}

TEST(ToSImmersion, threedimensional)
{
  const mln::image3d<mln::uint8> f = {
      {{0, 1, 2}, {3, 0, 1}},
      {{2, 0, 4}, {0, 0, 2}},
  };

  using R = mln::morpho::ToS::impl::irange<mln::uint8>;

  const mln::image3d<R> ref = {
      {{{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}},
       {{0, 3}, {0, 3}, {0, 1}, {0, 2}, {1, 2}},
       {{3, 3}, {0, 3}, {0, 0}, {0, 1}, {1, 1}}},
      {{{0, 2}, {0, 2}, {0, 1}, {0, 4}, {2, 4}},
       {{0, 3}, {0, 3}, {0, 1}, {0, 4}, {1, 4}},
       {{0, 3}, {0, 3}, {0, 0}, {0, 2}, {1, 2}}},
      {{{2, 2}, {0, 2}, {0, 0}, {0, 4}, {4, 4}},
       {{0, 2}, {0, 2}, {0, 0}, {0, 4}, {2, 4}},
       {{0, 0}, {0, 0}, {0, 0}, {0, 2}, {2, 2}}},
  };

  auto g = mln::morpho::ToS::impl::immersion(f);
  ASSERT_IMAGES_EQ(ref, g);
}


template <class T>
class ToSPropagation : public ::testing::Test
{
};

typedef ::testing::Types<mln::uint8, float> TestValueTypes;
TYPED_TEST_CASE(ToSPropagation, TestValueTypes);

TYPED_TEST(ToSPropagation, saddle_point)
{
  const mln::image2d<TypeParam> f = {{0, 0, 0, 0}, //
                                     {0, 2, 0, 0}, //
                                     {0, 0, 2, 0}, //
                                     {0, 0, 0, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0}, //
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
  const mln::image2d<TypeParam> f = {{0, 0, 0, 0}, //
                                     {0, 2, 3, 0}, //
                                     {0, 0, 2, 0}, //
                                     {1, 2, 0, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0}, //
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
  const mln::image3d<TypeParam> f = {{{0, 0, 0},  //
                                      {0, 2, 3},  //
                                      {0, 0, 2}}, //
                                     {{1, 0, 1},  //
                                      {0, 2, 1},  //
                                      {1, 0, 2}}};


  const mln::image3d<uint8> ref = {{{0, 0, 0, 0, 0},  //
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
  const mln::image2d<TypeParam> f = {{0, 255, 0, 255},     //
                                     {0, 255, 0, 0},       //
                                     {255, 255, 255, 255}, //
                                     {0, 255, 255, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 1, 1, 2, 2, 3}, //
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
  const mln::image2d<TypeParam> f = {{0, 255, 0, 255}, //
                                     {255, 0, 255, 0}, //
                                     {0, 255, 0, 255}, //
                                     {255, 0, 255, 0}};


  const mln::image2d<uint8> ref = {{0, 0, 1, 0, 0, 0, 1}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {1, 0, 0, 0, 1, 0, 0}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {0, 0, 1, 0, 0, 0, 1}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {1, 0, 0, 0, 1, 0, 0}};

  test_propagation(f, ref);
}

TYPED_TEST(ToSPropagation, input_has_no_border)
{
  std::initializer_list<std::initializer_list<TypeParam>> data = {{0, 0, 0, 0}, //
                                                                  {0, 2, 3, 0}, //
                                                                  {0, 0, 2, 0}, //
                                                                  {1, 2, 0, 0}};
  const mln::image2d<TypeParam>                           f(data, 0);

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {0, 0, 2, 2, 3, 0, 0}, //
                                   {0, 0, 0, 0, 2, 0, 0}, //
                                   {0, 0, 0, 0, 2, 0, 0}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {1, 1, 2, 0, 0, 0, 0}};

  test_propagation(f, ref);
}


TYPED_TEST(ToSPropagation, input_is_a_view)
{
  const mln::image2d<TypeParam> f = {{0, 0, 0, 0}, //
                                     {0, 2, 3, 0}, //
                                     {0, 0, 2, 0}, //
                                     {1, 2, 0, 0}};

  const mln::image2d<uint8> ref = {{0, 0, 0, 0, 0, 0, 0}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {0, 0, 2, 2, 3, 0, 0}, //
                                   {0, 0, 0, 0, 2, 0, 0}, //
                                   {0, 0, 0, 0, 2, 0, 0}, //
                                   {0, 0, 0, 0, 0, 0, 0}, //
                                   {1, 1, 2, 0, 0, 0, 0}};
  test_propagation(mln::imcast<TypeParam>(2 * f), ref);
}


TEST(ToSConstruction, saddle_point)
{
  const mln::image2d<uint8> f = {{0, 0, 0, 0}, //
                                 {0, 2, 0, 0}, //
                                 {0, 0, 2, 0}, //
                                 {0, 0, 0, 0}};

  const mln::point2d               a         = {0, 0};
  const mln::point2d               b         = {2, 2};
  const mln::point2d               c         = {4, 4};
  const mln::image2d<mln::point2d> ref_roots = {{a, a, a, a, a, a, a}, //
                                                {a, a, a, a, a, a, a}, //
                                                {a, a, b, a, a, a, a}, //
                                                {a, a, a, a, a, a, a}, //
                                                {a, a, a, a, c, a, a}, //
                                                {a, a, a, a, a, a, a}, //
                                                {a, a, a, a, a, a, a}};

  const mln::image2d<mln::point2d> ref_parent = {{a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}};
  test_construction(f, ref_parent, ref_roots);
}


TEST(ToSConstruction, two_branches_with_parallel_flooding)
{
  const mln::image2d<uint8> f = {{0, 0, 0, 0}, //
                                 {0, 2, 3, 0}, //
                                 {0, 0, 2, 0}, //
                                 {1, 2, 0, 0}};


  const mln::point2d a = {0, 0};
  const mln::point2d b = {2, 2};
  const mln::point2d c = {2, 4};
  const mln::point2d d = {6, 0};
  const mln::point2d e = {6, 2};

  const mln::image2d<mln::point2d> ref_parent = {{a, a, a, a, a, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, a, a, b, b, a, a}, //
                                                 {a, a, a, a, b, a, a}, //
                                                 {a, a, a, a, b, a, a}, //
                                                 {a, a, a, a, a, a, a}, //
                                                 {a, d, d, a, a, a, a}};

  const mln::image2d<mln::point2d> ref_roots = {{a, a, a, a, a, a, a}, //
                                                {a, a, a, a, a, a, a}, //
                                                {a, a, b, b, c, a, a}, //
                                                {a, a, a, a, b, a, a}, //
                                                {a, a, a, a, b, a, a}, //
                                                {a, a, a, a, a, a, a}, //
                                                {d, d, e, a, a, a, a}};

  test_construction(f, ref_parent, ref_roots);
}

TEST(ToSConstruction, branches_with_parallel_flooding_3d)
{
  const mln::image3d<mln::uint8> ima = {{{0, 0, 0},  //
                                         {0, 2, 3},  //
                                         {0, 0, 2}}, //
                                        {{1, 0, 1},  //
                                         {0, 2, 1},  //
                                         {1, 0, 2}}};

  const mln::point3d a = {0, 0, 0};
  const mln::point3d b = {2, 0, 0};
  const mln::point3d c = {2, 4, 0};
  const mln::point3d d = {1, 2, 3};
  const mln::point3d e = {0, 2, 2};
  const mln::point3d f = {0, 2, 4};

  const mln::image3d<mln::point3d> ref_roots = {{{a, a, a, a, a},  //
                                                 {a, a, a, a, a},  //
                                                 {a, a, e, e, f},  //
                                                 {a, a, a, a, e},  //
                                                 {a, a, a, a, e}}, //
                                                {{a, a, a, a, a},  //
                                                 {a, a, a, a, a},  //
                                                 {a, a, e, d, d},  //
                                                 {a, a, a, a, d},  //
                                                 {a, a, a, a, e}}, //
                                                {{b, a, a, a, d},  //
                                                 {a, a, a, a, d},  //
                                                 {a, a, e, d, d},  //
                                                 {a, a, a, a, d},  //
                                                 {c, a, a, a, e}}};


  const mln::image3d<mln::point3d> ref_parent = {{{a, a, a, a, a},  //
                                                  {a, a, a, a, a},  //
                                                  {a, a, d, e, e},  //
                                                  {a, a, a, a, e},  //
                                                  {a, a, a, a, e}}, //
                                                 {{a, a, a, a, a},  //
                                                  {a, a, a, a, a},  //
                                                  {a, a, e, a, d},  //
                                                  {a, a, a, a, d},  //
                                                  {a, a, a, a, e}}, //
                                                 {{a, a, a, a, d},  //
                                                  {a, a, a, a, d},  //
                                                  {a, a, e, d, d},  //
                                                  {a, a, a, a, d},  //
                                                  {a, a, a, a, e}}};

  test_construction(ima, ref_parent, ref_roots);
}

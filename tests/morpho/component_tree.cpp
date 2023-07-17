#include <mln/accu/accumulators/count.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/morpho/alphatree.hpp>
#include <mln/morpho/maxtree.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

TEST(Morpho, SaliencyAlphaTree)
{
  mln::image2d<std::uint8_t> input = {
      {1, 1, 3, 1}, //
      {2, 6, 8, 7}, //
      {5, 3, 2, 2}  //
  };

  mln::image2d<double> expected = {
      {0, 0, 0, 2, 0, 2, 0}, //
      {1, 3, 3, 3, 3, 3, 3}, //
      {0, 3, 0, 2, 0, 1, 0}, //
      {3, 3, 3, 3, 3, 3, 3}, //
      {0, 2, 0, 1, 0, 0, 0}  //
  };

  auto [t, nm] =
      mln::morpho::alphatree(input, mln::c4, [](auto a, auto b) -> double { return mln::functional::l2dist(a, b); });
  const auto res = t.saliency(nm, ::ranges::make_span(t.values.data(), t.values.size()));

  ASSERT_IMAGES_EQ_EXP(res, expected);
}



TEST(Morpho, Filtering_min)
{
  mln::image2d<std::uint8_t> input = {
      {0,  0,  0,  0, 0,  0}, //
      {0, 10, 12,  0, 0,  0}, //
      {0, 10, 15,  0, 0,  0}, //
      {0,  0,  0,  0, 8, 10}, //
      {0,  0,  0, 12, 6, 17}  //
  };

  mln::image2d<std::uint8_t> expected = {
      {0,  0,  0,  0, 0,  0}, //
      {0, 10, 10,  0, 0,  0}, //
      {0, 10, 10,  0, 0,  0}, //
      {0,  0,  0,  0, 8, 10}, //
      {0,  0,  0,  6, 6, 10}  //
  };

  {
    auto [tree, nodemap] = mln::morpho::maxtree(input, mln::c4);
    tree.filter(mln::morpho::CT_FILTER_MIN, [t=&tree](int x) { int a = t->values[x]; return a == 6 || a == 8 || a == 10; });
    auto rec = tree.reconstruct(nodemap);
    ASSERT_IMAGES_EQ_EXP(rec, expected);
  }
}

TEST(Morpho, Filtering_max)
{
  mln::image2d<std::uint8_t> input = {
      {0,  0,  0,  0, 0,  0}, //
      {0, 10, 12,  0, 0,  0}, //
      {0, 10, 15,  0, 0,  0}, //
      {0,  0,  0,  0, 8, 10}, //
      {0,  0,  0, 12, 6, 17}  //
  };

  mln::image2d<std::uint8_t> expected = {
      {0,  0,  0,  0, 0, 0}, //
      {0, 10, 12,  0, 0, 0}, //
      {0, 10, 12,  0, 0, 0}, //
      {0,  0,  0,  0, 8, 8}, //
      {0,  0,  0, 12, 6, 8}  //
  };

  {
    auto [tree, nodemap] = mln::morpho::maxtree(input, mln::c4);
    tree.filter(mln::morpho::CT_FILTER_MAX, [t=&tree](int x) { int a = t->values[x]; return a == 6 || a == 8 || a == 12; });
    auto rec = tree.reconstruct(nodemap);
    ASSERT_IMAGES_EQ_EXP(rec, expected);
  }
}

TEST(Morpho, Filtering_direct)
{
  mln::image2d<std::uint8_t> input = {
      {0,  0,  0,  0, 0,  0}, //
      {0, 10, 12,  0, 0,  0}, //
      {0, 10, 15,  0, 0,  0}, //
      {0,  0,  0,  0, 8, 10}, //
      {0,  0,  0, 12, 6, 17}  //
  };

  mln::image2d<std::uint8_t> expected = {
      {0,  0,  0,  0, 0, 0}, //
      {0,  0, 12,  0, 0, 0}, //
      {0,  0, 12,  0, 0, 0}, //
      {0,  0,  0,  0, 6, 6}, //
      {0,  0,  0, 12, 6, 17}  //
  };

  {
    auto [tree, nodemap] = mln::morpho::maxtree(input, mln::c4);
    tree.filter(mln::morpho::CT_FILTER_DIRECT, [t=&tree](int x) { int a = t->values[x]; return a == 6 || a == 12 || a == 17; });
    auto rec = tree.reconstruct(nodemap);
    ASSERT_IMAGES_EQ_EXP(rec, expected);
  }
}


TEST(Morpho, Filtering_subtractive)
{
  mln::image2d<std::uint8_t> input = {
      {0,  0,  0,  0, 0,  0}, //
      {0, 10, 12,  0, 0,  0}, //
      {0, 10, 15,  0, 0,  0}, //
      {0,  0,  0,  0, 8, 10}, //
      {0,  0,  0, 12, 6, 17}  //
  };

  mln::image2d<std::uint8_t> expected = {
      {0,  0,  0,  0, 0, 0}, //
      {0,  0,  2,  0, 0, 0}, //
      {0,  0,  5,  0, 0, 0}, //
      {0,  0,  0,  0, 6, 6}, //
      {0,  0,  0, 12, 6, 13}  //
  };

  {
    auto [tree, nodemap] = mln::morpho::maxtree(input, mln::c4);
    tree.filter(mln::morpho::CT_FILTER_SUBTRACTIVE, [t=&tree](int x) { int a = t->values[x]; return a == 6 || a >= 12; });
    auto rec = tree.reconstruct(nodemap);
    ASSERT_IMAGES_EQ_EXP(rec, expected);
  }
}

TEST(Morpho, HorizontalCut)
{
  mln::morpho::component_tree<int> t;
  t.parent = std::vector<int>{0, 0, 0, 1, 1, 2, 2, 2};
  t.values = std::vector<int>{3, 2, 1, 0, 0, 0, 0, 0};
  
  const mln::image2d<int> nodemap{
    {5, 5, 6, 3, 3},
    {5, 5, 6, 3, 4},
    {6, 6, 6, 7, 4}
  };

  const mln::image2d<int> ref_cut_1{
    {2, 2, 2, 3, 3},
    {2, 2, 2, 3, 4},
    {2, 2, 2, 2, 4}
  };

  const mln::image2d<int> ref_cut_2{
    {2, 2, 2, 1, 1},
    {2, 2, 2, 1, 1},
    {2, 2, 2, 2, 1}
  };

  const mln::image2d<int> ref_cut_3{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

  const auto cut_0 = t.horizontal_cut(0, nodemap);
  const auto cut_1 = t.horizontal_cut(1, nodemap);
  const auto cut_2 = t.horizontal_cut(2, nodemap);
  const auto cut_3 = t.horizontal_cut(3, nodemap);
  const auto cut_4 = t.horizontal_cut(4, nodemap);

  ASSERT_IMAGES_EQ_EXP(cut_0, nodemap);
  ASSERT_IMAGES_EQ_EXP(cut_1, ref_cut_1);
  ASSERT_IMAGES_EQ_EXP(cut_2, ref_cut_2);
  ASSERT_IMAGES_EQ_EXP(cut_3, ref_cut_3);
  ASSERT_IMAGES_EQ_EXP(cut_4, ref_cut_3);
}

TEST(Morpho, HorizontalCut_AfterFiltering)
{
  mln::morpho::component_tree<int> t;
  t.parent = std::vector<int>{0, 0, 0, 1, 1, 2, 2, 2};
  t.values = std::vector<int>{3, 2, 1, 0, 0, 0, 0, 0};
  
  mln::image2d<int> nodemap{
    {5, 5, 6, 3, 3},
    {5, 5, 6, 3, 4},
    {6, 6, 6, 7, 4}
  };

  const mln::image2d<int> ref_cut_0{
    {5, 5, 6, 3, 3},
    {5, 5, 6, 3, 4},
    {6, 6, 6, 2, 4}
  };

  const mln::image2d<int> ref_cut_1{
    {2, 2, 2, 3, 3},
    {2, 2, 2, 3, 4},
    {2, 2, 2, 2, 4}
  };

  const mln::image2d<int> ref_cut_2{
    {2, 2, 2, 1, 1},
    {2, 2, 2, 1, 1},
    {2, 2, 2, 2, 1}
  };

  const mln::image2d<int> ref_cut_3{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  };

  const auto area = t.compute_attribute_on_points(nodemap, mln::accu::accumulators::count{});
  t.filter(mln::morpho::ct_filtering::CT_FILTER_DIRECT, nodemap, [&area](int n) { return area[n] > 1; });

  const auto cut_0 = t.horizontal_cut(0, nodemap);
  const auto cut_1 = t.horizontal_cut(1, nodemap);
  const auto cut_2 = t.horizontal_cut(2, nodemap);
  const auto cut_3 = t.horizontal_cut(3, nodemap);
  const auto cut_4 = t.horizontal_cut(4, nodemap);

  ASSERT_IMAGES_EQ_EXP(nodemap, ref_cut_0);
  ASSERT_IMAGES_EQ_EXP(cut_0, ref_cut_0);
  ASSERT_IMAGES_EQ_EXP(cut_1, ref_cut_1);
  ASSERT_IMAGES_EQ_EXP(cut_2, ref_cut_2);
  ASSERT_IMAGES_EQ_EXP(cut_3, ref_cut_3);
  ASSERT_IMAGES_EQ_EXP(cut_4, ref_cut_3);
}
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
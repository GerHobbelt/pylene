#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include "range2d.hpp"

#include <vector>

#include <gtest/gtest.h>

TEST(NewDesignChangeVal, CanChangeVal)
{
  mln::image2d<mln::uint8> f(5, 5);
  mln::image2d<mln::uint8> g(5, 5);

  mln::iota(f, 0);
  mln::fill(g, 0);

  int i = 0;

  auto rng = values_of<mln::uint8>(g);
  mln_foreach_new(auto& v, rng)
  {
    v = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_forall(f_px, g_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
  }
}


TEST(NewDesignChangePix, CanChangePix)
{
  mln::image2d<mln::uint8> f(5, 5);
  mln::image2d<mln::uint8> g(5, 5);

  mln::iota(f, 0);
  mln::fill(g, 0);
  int i = 0;
  auto rng = pixels_of(g);
  mln_foreach_new(auto pix, rng)
  {
    pix.val() = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_forall(f_px, g_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
  }
}

TEST(NewDesignChangeVal_Multiple_Iterator, CanChangeVal)
{
  mln::image2d<mln::uint8> f(5, 5);
  mln::image2d<mln::uint8> g(5, 5);
  mln::image2d<mln::uint8> h(5, 5);

  mln::iota(f, 0);
  mln::fill(g, 0);
  mln::fill(h, 0);

  int i = 0;

  auto rng = values_of(g);
  auto rng1 = values_of(h);
  mln_foreach_new((auto&& [v, v1]), ranges::view::zip(rng, rng1))
  {
    v = i;
    v1 = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_pixter(h_px, h);
  mln_forall(f_px, g_px, h_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
    EXPECT_EQ(f_px->val(), h_px->val());
  }
}


TEST(NewDesignChangePix_Multiple_Iterator, CanChangePix)
{
  mln::image2d<mln::uint8> f(5, 5);
  mln::image2d<mln::uint8> g(5, 5);
  mln::image2d<mln::uint8> h(5, 5);

  mln::iota(f, 0);
  mln::fill(g, 0);
  mln::fill(h, 0);

  int i = 0;

  auto rng = pixels_of(g);
  auto rng1 = pixels_of(h);

  mln_foreach_new((auto&& [pix, pix1]), ranges::view::zip(rng, rng1))
  {
    pix.val() = i;
    pix1.val() = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_pixter(h_px, h);
  mln_forall(f_px, g_px, h_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
    EXPECT_EQ(f_px->val(), h_px->val());
  }
}

TEST(NewDesignVal_IterWithoutMacro, Not_On_Stride)
{
  int width = 5;
  int height = 5;
  mln::image2d<mln::uint8> f(width, height);
  mln::image2d<mln::uint8> g(width, height);
  mln::iota(g, 0);

  int i = 0;

  auto rng = values_of(f);
  for (auto& val: rng)
  {
    val = i++;
  }
  EXPECT_EQ(width * height, i);

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_forall(f_px, g_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
  }
}

TEST(NewDesignPix_IterWithoutMacro, Not_On_Stride)
{
  int width = 5;
  int height = 5;
  mln::image2d<mln::uint8> f(width, height);
  mln::image2d<mln::uint8> g(width, height);
  mln::iota(g, 0);

  int i = 0;

  auto rng = pixels_of(f);
  for (auto& pix: rng)
  {
    pix.val() = i++;
  }
  EXPECT_EQ(width * height, i);

  mln_pixter(f_px, f);
  mln_pixter(g_px, g);
  mln_forall(f_px, g_px)
  {
    EXPECT_EQ(f_px->val(), g_px->val());
  }
}

TEST(NewDesignVal_ZipSegAndNotSeg, CanChangeVal)
{
  int width = 5;
  int height = 5;
  mln::image2d<mln::uint8> f(width, height);
  auto g = std::vector<mln::uint8>(width * height, 0);
  mln::image2d<mln::uint8> h(width, height);

  mln::fill(f, 0);
  mln::iota(h, 0);

  int i = 0;

  auto rng = values_of(f);
  mln_foreach_new((auto&& [val, val1]), ranges::view::zip(rng, g))
  {
    val = i;
    val1 = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(h_px, h);
  mln_forall(f_px, h_px)
  {
    EXPECT_EQ(f_px->val(), h_px->val());
  }

  mln::uint8* f_buf = &f.at(0,0);
  mln::uint8* g_buf = &g[0];
  std::ptrdiff_t offset = 0;
  for (unsigned int i = 0; i < f.nrows(); i++)
  {
    for (unsigned int j = 0; j < f.ncols(); j++)
    {
      EXPECT_EQ(g_buf[i * f.ncols() + j], f_buf[i * f.ncols() + j + offset]);
    }
    offset += f.index_strides()[0] - f.ncols();
  }
}

TEST(NewDesignPix_ZipSegAndNotSeg, CanChangePix)
{
  int width = 5;
  int height = 5;
  mln::image2d<mln::uint8> f(width, height);
  auto g = std::vector<mln::uint8>(width * height, 0);
  mln::image2d<mln::uint8> h(width, height);

  mln::fill(f, 0);
  mln::iota(h, 0);

  int i = 0;

  auto rng = pixels_of(f);
  mln_foreach_new((auto&& [pix, val1]), ranges::view::zip(rng, g))
  {
    pix.val() = i;
    val1 = i++;
  }

  mln_pixter(f_px, f);
  mln_pixter(h_px, h);
  mln_forall(f_px, h_px)
  {
    EXPECT_EQ(f_px->val(), h_px->val());
  }

  mln::uint8* f_buf = &f.at(0,0);
  mln::uint8* g_buf = &g[0];
  std::ptrdiff_t offset = 0;
  for (unsigned int i = 0; i < f.nrows(); i++)
  {
    for (unsigned int j = 0; j < f.ncols(); j++)
    {
      EXPECT_EQ(g_buf[i * f.ncols() + j], f_buf[i * f.ncols() + j + offset]);
    }
    offset += f.index_strides()[0] - f.ncols();
  }
}

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
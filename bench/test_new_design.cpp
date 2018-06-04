#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include "range2d.hpp"

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

  auto rng = values_of<mln::uint8>(g);
  auto rng1 = values_of<mln::uint8>(h);
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

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
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

  mln_pixter(pxIn, f);
  mln_pixter(pxOut, g);
  mln_forall(pxIn, pxOut)
  {
    EXPECT_EQ(pxOut->val(), pxIn->val());
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

  mln_pixter(pxIn, f);
  mln_pixter(pxOut, g);
  mln_forall(pxIn, pxOut)
  {
    EXPECT_EQ(pxOut->val(), pxIn->val());
  }
}

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
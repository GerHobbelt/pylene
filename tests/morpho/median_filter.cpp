#include <mln/core/algorithm/fill.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/median_filter.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

using namespace mln;

image2d<uint8> naive_median(const image2d<uint8>& f, se::rect2d win, int sz)
{
  mln_entering("naive_median");

  image2d<uint8> g;
  resize(g, f);

  mln_pixter(px, qx, f, g);
  mln_iter(nx, win(px));

  std::vector<uint8> V;

  mln_forall (px, qx)
  {
    V.clear();
    mln_forall (nx)
      V.push_back(nx->val());
    std::sort(V.begin(), V.end());
    qx->val() = V[sz / 2];
  }

  mln_exiting();
  return g;
}

TEST(Morpho, median_filter_median_filter_0)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("lena.pgm"), ima);

  { // Fast: border wide enough
    mln::se::rect2d win(7, 7);
    auto            out  = morpho::median_filter(ima, win);
    auto            out2 = naive_median(ima, win, 49);
    ASSERT_IMAGES_EQ(out2, out);
  }
}

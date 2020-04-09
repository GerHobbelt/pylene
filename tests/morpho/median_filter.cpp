#include <mln/morpho/experimental/median_filter.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/range/view/zip.hpp>

#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>


mln::experimental::image2d<uint8_t> naive_median(const mln::experimental::image2d<uint8_t>& f,
                                                 mln::experimental::se::rect2d win, int sz)
{
  mln_entering("naive_median");

  mln::experimental::image2d<uint8_t> g;
  mln::resize(g, f);

  std::vector<uint8_t> V;
  auto zz = mln::ranges::view::zip(f.new_pixels(), g.new_values());
  for (auto&& r : zz.rows())
    for (auto&& [px, vout] : r)
    {
      V.clear();
      for (auto qx : win(px))
        V.push_back(qx.val());
      std::partial_sort(V.begin(), V.begin() + sz / 2 + 1, V.end());
      vout = V[sz / 2];
    }
  return g;
}

TEST(Morpho, median_filter)
{
  mln::experimental::image2d<uint8_t> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("lena.pgm"), ima);

  { // Fast: border wide enough
    mln::experimental::se::rect2d win(7, 7);
    auto                          out  = mln::morpho::experimental::median_filter(ima, win, mln::extension::bm::native::mirror());
    auto                          out2 = naive_median(ima, win, 49);
    ASSERT_IMAGES_EQ_EXP(out2, out);
  }
}

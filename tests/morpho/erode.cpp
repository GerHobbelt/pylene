#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/experimental/dilation.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/experimental/imread.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


// Check that dilation/erosion are adjunctions
TEST(Morpho, erode_0)
{
  mln::experimental::image2d<uint8_t> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::experimental::image2d<uint8_t> out1, out2;
  mln::experimental::se::disc win(3);

  auto comp = [](uint8_t x) -> uint8_t { return 255 - x; };

  out1 = mln::morpho::experimental::dilation(ima, win);
  out2 = mln::morpho::experimental::erosion(mln::view::transform(ima, comp), win);

  ASSERT_IMAGES_EQ_EXP(out2, mln::view::transform(out1, comp));
}

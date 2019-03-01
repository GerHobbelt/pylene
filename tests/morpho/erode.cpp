#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/structural/dilate.hpp>
#include <mln/morpho/structural/erode.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

using namespace mln;

// Check that dilation/erosion are adjunctions
TEST(Morpho, erode_0)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  image2d<uint8> out1, out2;
  mln::se::disc  win(3);

  auto comp = [](uint8 x) -> uint8 { return 255 - x; };

  out1 = morpho::structural::dilate(ima, win);
  out2 = morpho::structural::erode(imtransform(ima, comp), win);

  ASSERT_IMAGES_EQ(out2, imtransform(out1, comp));
}

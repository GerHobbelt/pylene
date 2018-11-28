#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/structural/gradient.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


using namespace mln;

TEST(Morpho, gradient_gradient_0)
{
  image2d<uint8> ima(10, 10);
  iota(ima, 10);

  { // Fast: border wide enough
    mln::se::rect2d win(3, 1);
    auto            out = morpho::structural::gradient(ima, win);

    static_assert(std::is_same<decltype(out)::value_type, int>::value, "Error integral promotion should give int.");
    ASSERT_TRUE(all(lor(out == 1, out == 2)));
  }
}

TEST(Morpho, gradient_gradient_1)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  { // Fast: border wide enough
    mln::se::rect2d win(7, 7);
    auto            out = morpho::structural::gradient(ima, win);
  }
}

// Border is not wide enough => call dilate + erode
TEST(Morpho, gradient_gradient_2)
{
  image2d<uint8> ima(0);
  image2d<uint8> ima2;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima2);

  mln::se::rect2d win(3, 3);
  auto            out1 = morpho::structural::gradient(ima, win);
  auto            out2 = morpho::structural::gradient(ima2, win);
  ASSERT_TRUE(all(out1 == out2));
}

// Dilation on a with a vmorph / binary case
TEST(Morpho, gradient_gradient_3)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  // Morpher has no extension
  mln::se::rect2d win(3, 3);
  auto            out = morpho::structural::gradient(ima > 128, win);
}

// Dilation on a with a vmorph / binary case
TEST(Morpho, gradient_gradient_4)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(3, 3);
  image2d<uint8>  out;
  resize(out, ima).init(0);
  auto tmp = out | where(ima > 128);
  morpho::structural::gradient(ima | where(ima > 128), win, std::less<uint8>(), functional::l2norm_t<uint8>(), tmp);
}

// On colors
TEST(Morpho, gradient_gradient_5)
{
  image2d<rgb8> ima;
  image2d<rgb8> ima2(0);
  io::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);
  io::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima2);

  mln::se::rect2d win(3, 3);
  auto            out1 = morpho::structural::gradient(ima, win);
  auto            out2 = morpho::structural::gradient(ima2, win);
  ASSERT_TRUE(all(out1 == out2));
}

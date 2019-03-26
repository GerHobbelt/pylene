#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/structural/closing.hpp>
#include <mln/morpho/structural/opening.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

using namespace mln;

TEST(Morpho, opening_closing_opening_0)
{
  using namespace mln::view::ops;

  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(3, 3);
  {
    auto out1 = morpho::structural::opening(ima, win);
    auto out2 = morpho::structural::closing(ima, win);

    ASSERT_TRUE(all_of(out1 <= ima)); // anti-extensive
    ASSERT_TRUE(all_of(out2 >= ima)); // extensive
  }
}

TEST(Morpho, opening_closing_opening_1)
{
  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto            comp = [](uint8 x) -> uint8 { return 255 - x; };
  mln::se::rect2d win(3, 3);
  {
    auto out1 = morpho::structural::opening(imtransform(ima, comp), win);
    auto out2 = morpho::structural::closing(ima, win);

    ASSERT_IMAGES_EQ(out1, imtransform(out2, comp)); // duality
  }
}

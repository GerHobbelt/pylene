#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/saturate.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


TEST(Morpho, saturate)
{

  using namespace mln;

  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("squares.pgm"), ima);

  image2d<bool> out = morpho::saturate(ima == 154, c4, point2d{0, 0});
  ASSERT_TRUE(all(out == (lor(lor(ima == 154, ima == 251), ima == 12))));

  morpho::saturate(ima == 130, c4, point2d{0, 0}, out);
  ASSERT_TRUE(all(out == (ima >= 12)));

  // pinf \in A => sat(A) == domain(f)
  morpho::saturate(ima == 195, c4, point2d{69, 45}, out);
  ASSERT_TRUE(all(out == true));

  // io::imprint(out);
}

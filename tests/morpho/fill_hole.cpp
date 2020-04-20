#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/fill_hole.hpp>
#include <mln/core/algorithm/all_of.hpp>

#include <fixtures/ImagePath/image_path.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>



TEST(Morpho, fill_hole)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;

  mln::io::imread(fixtures::ImagePath::concat_with_filename("squares.pgm"), ima);

  mln::image2d<bool> out = mln::morpho::fill_hole(ima == 154, mln::c4, {0, 0});
  ASSERT_IMAGES_EQ_EXP(out, ((ima == 154) || (ima == 251) || (ima == 12)));

  mln::morpho::fill_hole(ima == 130, mln::c4, {0, 0}, out);
  ASSERT_IMAGES_EQ_EXP(out, (ima >= 12));

  // pinf \in A => sat(A) == domain(f)
  mln::morpho::fill_hole(ima == 195, mln::c4, {45, 69}, out);
  ASSERT_TRUE(mln::all_of(out));
}

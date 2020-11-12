#include <mln/core/canvas/morpho_pipeline.hpp>

#include <mln/morpho/gradient.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/rect2d.hpp>

#include <functional>

#include <fixtures/ImagePath/image_path.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <mln/io/imread.hpp>
#include <gtest/gtest.h>

TEST(Morpho, gradient_color)
{
  using namespace mln::view::ops;

  mln::image2d<mln::uint8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(3, 3);
  auto grad1 = mln::morpho::internal_gradient(ima, win);
  auto grad2 = mln::morpho::MorphoPipeline(mln::morpho::details::e_MorphoPipelineOperation::Grad_int, ima, win).execute();

  ASSERT_TRUE(mln::all_of(grad2 == grad1));
}
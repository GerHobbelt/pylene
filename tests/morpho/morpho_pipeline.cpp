#include <mln/core/canvas/morpho_pipeline.hpp>

#include <mln/morpho/gradient.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/rect2d.hpp>

#include <functional>
#include <tbb/task_scheduler_init.h>

#include <fixtures/ImagePath/image_path.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <mln/io/imread.hpp>
#include <gtest/gtest.h>

/*
    Closing,
    Opening,
    Grad_thick,
    Grad_ext,
    Grad_int,
    Top_hat,
    Bot_hat
*/

/*TEST(Morpho, gradient_ext_endtoend)
{
  using namespace mln::view::ops;

  mln::image2d<mln::uint8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(3, 3);
  auto grad1 = mln::morpho::external_gradient(ima, win);
  auto grad2 = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Grad_ext, ima, win).execute();

  ASSERT_TRUE(mln::all_of(grad2 == grad1));
}

TEST(Morpho, gradient_int_endtoend)
{
  using namespace mln::view::ops;

  mln::image2d<mln::uint8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(3, 3);

  tbb::task_scheduler_init init;
  
  auto grad1 = mln::morpho::internal_gradient(ima, win);
  auto grad2 = mln::morpho::MorphoPipeline(mln::morpho::e_MorphoPipelineOperation::Grad_int, ima, win).execute(); //TODO mln::morpho::pipeline::internal_gradient

  ASSERT_TRUE(mln::all_of(grad2 == grad1));
}*/

TEST(Morpho, foo)
{
  tbb::task_scheduler_init init;
  using namespace mln::view::ops;
  mln::image2d<mln::uint8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  mln::se::rect2d win(3, 3);

  typedef mln::image2d<mln::uint8> (*functype)(mln::image2d<mln::uint8>&, const mln::se::rect2d&);
  functype dilate = mln::morpho::parallel::dilation;
  std::function<mln::image2d<mln::uint8>(mln::image2d<mln::uint8>&, const mln::se::rect2d&)> f = dilate;

  auto& ref = ima;
  mln::image2d<mln::uint8> d1 = f(ref, win);
  mln::image2d<mln::uint8> d2 = mln::morpho::parallel::dilation(ima, win);
  ASSERT_TRUE(mln::all_of(d1 == d2));
}
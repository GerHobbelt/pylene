#include <mln/labeling/blobs.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

TEST(Labeling, blobs_U)
{
  using namespace mln::view::ops;

  mln::image2d<bool> ima = {{0, 1, 0, 0, 0, 1, 0}, //
                            {0, 1, 0, 1, 0, 1, 0}, //
                            {0, 1, 0, 0, 0, 1, 0}, //
                            {0, 1, 1, 1, 1, 1, 0}, //
                            {0, 1, 1, 1, 1, 1, 0}};

  mln::image2d<uint8_t> ref = {{0, 1, 0, 0, 0, 1, 0}, //
                               {0, 1, 0, 2, 0, 1, 0}, //
                               {0, 1, 0, 0, 0, 1, 0}, //
                               {0, 1, 1, 1, 1, 1, 0}, //
                               {0, 1, 1, 1, 1, 1, 0}};


  int nlabel;
  auto out = mln::labeling::blobs<uint8_t>(ima, mln::c4, nlabel);
  ASSERT_EQ(nlabel, 2);
  ASSERT_IMAGES_EQ_EXP(ref, out);
}



TEST(Labeling, blobs_fast)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima(5, 5);
  mln::iota(ima, 0);

  auto mask = mln::transform(ima, [](int x) { return x % 2 == 0; });

  int nlabel;
  auto out = mln::labeling::blobs<uint8_t>(mask, mln::c4, nlabel);
  ASSERT_EQ(nlabel, 13);
  ASSERT_IMAGES_EQ_EXP(mask, out > 0);
}

TEST(Labeling, blobs_custom)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima(5, 5);
  mln::iota(ima, 0);

  int nlabel;
  {
    auto out = mln::labeling::blobs<uint8_t>(ima % 2 == 0, mln::c4, nlabel);
    ASSERT_IMAGES_EQ_EXP(ima % 2 == 0, out > 0);
    ASSERT_EQ(nlabel, 13);
  }

  {
    auto out = mln::labeling::blobs<uint8_t>(ima % 2 == 0, mln::c8, nlabel);
    ASSERT_IMAGES_EQ_EXP(ima % 2 == 0, out > 0);
    ASSERT_EQ(nlabel, 1u);
  }
}

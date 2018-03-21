#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#ifndef MLN_IMG_PATH
#error "MLN_IMG_PATH must be defined."
#endif

#include <gtest/gtest.h>
#include <tests/helpers.hpp>

TEST(IO, FreeImage_pgm)
{
  using namespace mln;

  image2d<uint8> ima;
  image2d<uint8> ref(5, 5);

  iota(ref, 1);
  io::imread(MLN_IMG_PATH "/iota2d.pgm", ima);
  ASSERT_IMAGES_EQ(ref, ima);
  io::imsave(ref, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ(ref, ima);
}

TEST(IO, FreeImage_ppm)
{
  using namespace mln;

  image2d<rgb8> ima;
  image2d<rgb8> ref(5, 5);

  mln_foreach (auto pix, ref.pixels())
  {
    pix.val()[0] = uint8(pix.point()[0]);
    pix.val()[1] = uint8(pix.point()[1]);
    pix.val()[2] = 0;
  }

  io::imread(MLN_IMG_PATH "/iota2d.ppm", ima);
  ASSERT_IMAGES_EQ(ref, ima);
  io::imsave(ima, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ(ref, ima);
}

TEST(IO, FreeImage_pbm)
{
  using namespace mln;

  image2d<bool> ima;
  image2d<bool> ref(5, 5);

  mln_foreach (point2d p, ref.domain())
    ref(p) = ((p[0] % 2) == (p[1] % 2));

  io::imsave(ref, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(equal(ima, ref));
}

TEST(IO, FreeImage_slow_pgm)
{
  using namespace mln;

  image2d<uint8> ima;
  image2d<uint8> ref(5, 5);

  iota(ref, 1);
  io::imread(MLN_IMG_PATH "/iota2d.pgm", ima);
  ASSERT_IMAGES_EQ(ref, ima);

  auto tmp = 2u * ref;
  io::imsave(imcast<uint32>(tmp), "test.tiff");
  image2d<unsigned> ima2;
  io::imread("test.tiff", ima2);
  ASSERT_IMAGES_EQ(tmp, ima2);
}

TEST(IO, FreeImage_slow_ppm)
{
  using namespace mln;

  image2d<rgb8> ima;
  image2d<rgb8> ref(5, 5);

  mln_foreach (const image2d<rgb8>::pixel_type& pix, ref.pixels())
  {
    pix.val()[0] = uint8(pix.point()[0]);
    pix.val()[1] = uint8(pix.point()[1]);
    pix.val()[2] = 0;
  }

  io::imread(MLN_IMG_PATH "/iota2d.ppm", ima);
  ASSERT_TRUE(equal(ima, ref));

  auto tmp = 2u * ref;
  io::imsave(imcast<rgb8>(tmp), "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(equal(ima, 2 * ref));
}

TEST(IO, FreeImage_slow_pbm)
{
  using namespace mln;

  image2d<bool> ima;
  image2d<bool> ref(5, 5);

  mln_foreach (point2d p, ref.domain())
    ref(p) = ((p[0] % 2) == (p[1] % 2));

  io::imsave(lnot(ref), "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(equal(ima, lnot(ref)));
}

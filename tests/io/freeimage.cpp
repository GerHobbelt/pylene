#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

TEST(IO, FreeImage_pgm)
{
  using namespace mln;

  image2d<uint8> ima;
  image2d<uint8> ref(5, 5);

  iota(ref, 1);
  io::imread(fixtures::ImagePath::concat_with_filename("iota2d.pgm"), ima);
  ASSERT_TRUE(equal(ima, ref));
  io::imsave(ref, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(equal(ima, ref));
}

TEST(IO, FreeImage_ppm)
{
  using namespace mln;

  image2d<rgb8> ima;
  image2d<rgb8> ref(5, 5);

  mln_foreach (const image2d<rgb8>::pixel_type& pix, ref.pixels())
  {
    pix.val()[0] = uint8(pix.point()[0]);
    pix.val()[1] = uint8(pix.point()[1]);
  }

  io::imread(fixtures::ImagePath::concat_with_filename("iota2d.ppm"), ima);
  ASSERT_TRUE(equal(ima, ref));
  io::imsave(ima, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(equal(ima, ref));
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
  using namespace mln::view::ops;

  image2d<uint8> ima;
  image2d<uint8> ref(5, 5);

  iota(ref, 1);
  io::imread(fixtures::ImagePath::concat_with_filename("iota2d.pgm"), ima);
  ASSERT_TRUE(equal(ima, ref));

  auto imax2 = 2u * ref;
  io::experimental::imsave(view::cast<uint32>(imax2), "test.tiff");

  image2d<unsigned> ima2;
  io::imread("test.tiff", ima2);
  ASSERT_TRUE(mln::equal(ima2, imax2));
}

TEST(IO, FreeImage_slow_ppm)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<rgb8> ima;
  image2d<rgb8> ref(5, 5);

  mln_foreach (const image2d<rgb8>::pixel_type& pix, ref.pixels())
  {
    pix.val()[0] = uint8(pix.point()[0]);
    pix.val()[1] = uint8(pix.point()[1]);
  }

  io::imread(fixtures::ImagePath::concat_with_filename("iota2d.ppm"), ima);
  ASSERT_TRUE(equal(ima, ref));

  auto tmp = 2u * ref;
  io::experimental::imsave(view::cast<rgb8>(tmp), "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(mln::equal(ima, 2 * ref));
}

TEST(IO, FreeImage_slow_pbm)
{
  using namespace mln;
  using namespace mln::view::ops;

  image2d<bool> ima;
  image2d<bool> ref(5, 5);

  mln_foreach (point2d p, ref.domain())
    ref(p) = ((p[0] % 2) == (p[1] % 2));

  io::experimental::imsave(not ref, "test.tiff");
  io::imread("test.tiff", ima);
  ASSERT_TRUE(mln::equal(ima, not ref));
}

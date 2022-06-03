#include <mln/io/oiio/imread.hpp>
#include <mln/io/oiio/imsave.hpp>


#include <mln/core/algorithm/iota.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

// *******************************
// * From FreeImage plugin tests *
// *******************************

TEST(IO, OpenImageIO_pgm)
{
  mln::image2d<uint8_t> ima;
  mln::image2d<uint8_t> ref(5, 5);

  mln::iota(ref, 1);
  mln::io::oiio::imread(fixtures::ImagePath::concat_with_filename("iota2d.pgm"), ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);


  mln::io::oiio::imsave(ref, "test.tiff");
  mln::io::oiio::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}

// This test fails on Windows due to CLRF in the pgm
#ifdef _WIN32
TEST(IO, DISABLED_OpenImageIO_pgm_fly)
#else
TEST(IO, OpenImageIO_pgm_fly)
#endif
{
  const mln::image2d<uint8_t> ref = {
      {128, 124, 150, 137, 106}, //
      {116, 128, 156, 165, 117}, //
      {117, 90, 131, 108, 151},  //
      {107, 87, 118, 109, 167},  //
      {107, 73, 125, 157, 117},  //
  };
  mln::image2d<uint8_t> ima;
  mln::io::oiio::imread(fixtures::ImagePath::concat_with_filename("fly.pgm"), ima);

  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}


TEST(IO, OpenImageIO_ppm)
{
  mln::image2d<mln::rgb8> ima;
  mln::image2d<mln::rgb8> ref(5, 5);

  mln_foreach (auto&& pix, ref.pixels())
  {
    pix.val()[0] = uint8_t(pix.point().y());
    pix.val()[1] = uint8_t(pix.point().x());
    pix.val()[2] = 0;
  }

  mln::io::oiio::imread(fixtures::ImagePath::concat_with_filename("iota2d.ppm"), ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);
  mln::io::oiio::imsave(ima, "test.tiff");
  mln::io::oiio::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}


TEST(IO, OpenImageIO_pbm)
{
  mln::image2d<bool> ima;
  mln::image2d<bool> ref(5, 5);

  mln_foreach (auto p, ref.domain())
    ref(p) = ((p.x() % 2) == (p.y() % 2));

  mln::io::oiio::imsave(ref, "test.pbm");
  mln::io::oiio::imread("test.pbm", ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}

TEST(IO, OpenImageIO_slow_pgm)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;
  mln::image2d<uint8_t> ref(5, 5);

  mln::iota(ref, 1);
  mln::io::oiio::imread(fixtures::ImagePath::concat_with_filename("iota2d.pgm"), ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);

  auto imax2 = 2u * ref;
  mln::io::oiio::imsave(mln::view::cast<uint32_t>(imax2), "test.tiff");

  mln::image2d<unsigned> ima2;
  mln::io::oiio::imread("test.tiff", ima2);
  ASSERT_IMAGES_EQ_EXP2(ima2, imax2, fixtures::ImageCompare::COMPARE_DOMAIN);
}

TEST(IO, OpenImageIO_slow_ppm)
{
  using namespace mln::view::ops;

  mln::image2d<mln::rgb8> ima;
  mln::image2d<mln::rgb8> ref(5, 5);

  mln_foreach (auto&& pix, ref.pixels())
  {
    pix.val()[0] = uint8_t(pix.point().y());
    pix.val()[1] = uint8_t(pix.point().x());
    pix.val()[2] = 0;
  }

  mln::io::oiio::imread(fixtures::ImagePath::concat_with_filename("iota2d.ppm"), ima);
  ASSERT_IMAGES_EQ_EXP2(ima, ref, fixtures::ImageCompare::COMPARE_DOMAIN);

  auto tmp = 2u * ref;
  mln::io::oiio::imsave(mln::view::cast<mln::rgb8>(tmp), "test.tiff");
  mln::io::oiio::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ_EXP2(ima, 2 * ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}

TEST(IO, OpenImageIO_slow_pbm)
{
  using namespace mln::view::ops;

  mln::image2d<bool> ima;
  mln::image2d<bool> ref(5, 5);

  mln_foreach (auto p, ref.domain())
    ref(p) = ((p.x() % 2) == (p.y() % 2));

  mln::io::oiio::imsave(not ref, "test.tiff");
  mln::io::oiio::imread("test.tiff", ima);
  ASSERT_IMAGES_EQ_EXP2(ima, not ref, fixtures::ImageCompare::COMPARE_DOMAIN);
}

// *************
// * New tests *
// *************

template <typename T>
class OpenImageIOUParameterizedTest : public ::testing::TestWithParam<std::string_view>
{
protected:
  const mln::image2d<T> ref{
      {1, 2, 3}, //
      {4, 5, 6}, //
      {7, 8, 9}  //
  };
};

using OpenImageIOUInt8ParameterizedTest = OpenImageIOUParameterizedTest<std::uint8_t>;
TEST_P(OpenImageIOUInt8ParameterizedTest, OpenImageIOUInt8)
{
  const auto                 ext = GetParam();
  mln::image2d<std::uint8_t> tested;
  mln::io::oiio::imsave(ref, fmt::format("test.{}", ext));
  mln::io::oiio::imread(fmt::format("test.{}", ext), tested);
  ASSERT_IMAGES_EQ_EXP2(ref, tested, fixtures::ImageCompare::COMPARE_DOMAIN);
}

INSTANTIATE_TEST_SUITE_P(IO, OpenImageIOUInt8ParameterizedTest,
                         ::testing::Values("png", "pgm", "tiff", "bmp", "sgi", "tga"));

using OpenImageIOUInt16ParameterizedTest = OpenImageIOUParameterizedTest<std::uint16_t>;
TEST_P(OpenImageIOUInt16ParameterizedTest, OpenImageIOUInt16)
{
  const auto                  ext = GetParam();
  mln::image2d<std::uint16_t> tested;
  mln::io::oiio::imsave(ref, fmt::format("test.{}", ext));
  mln::io::oiio::imread(fmt::format("test.{}", ext), tested);
  ASSERT_IMAGES_EQ_EXP2(ref, tested, fixtures::ImageCompare::COMPARE_DOMAIN);
}

// Do not test pgm since the type is forced to UINT8 with a scaling in OIIO
INSTANTIATE_TEST_SUITE_P(IO, OpenImageIOUInt16ParameterizedTest, ::testing::Values("sgi", "tiff", "png"));

class OpenImageIOParameterizedTestRGB8 : public ::testing::TestWithParam<std::string_view>
{
protected:
  const mln::image2d<mln::rgb8> ref{
      {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},          //
      {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}}, //
      {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}  //
  };
};

TEST_P(OpenImageIOParameterizedTestRGB8, OpenImageIORGB8)
{
  const auto              ext = GetParam();
  mln::image2d<mln::rgb8> tested;
  mln::io::oiio::imsave(ref, fmt::format("test.{}", ext));
  mln::io::oiio::imread(fmt::format("test.{}", ext), tested);
  ASSERT_IMAGES_EQ_EXP2(ref, tested, fixtures::ImageCompare::COMPARE_DOMAIN);
}

INSTANTIATE_TEST_SUITE_P(IO, OpenImageIOParameterizedTestRGB8,
                         ::testing::Values("png", "ppm", "tiff", "bmp", "tga", "sgi"));

class OpenImageIOParameterizedTestBool : public ::testing::TestWithParam<std::string_view>
{
protected:
  const mln::image2d<bool> ref{
      {true, false, true, false, true},  //
      {false, false, true, false, true}, //
      {true, false, false, false, true}, //
      {true, true, false, true, false},  //
      {false, true, true, false, true},  //
  };
};

TEST_P(OpenImageIOParameterizedTestBool, OpenImageIOBool)
{
  const auto         ext = GetParam();
  mln::image2d<bool> tested;
  mln::io::oiio::imsave(ref, fmt::format("test.{}", ext));
  mln::io::oiio::imread(fmt::format("test.{}", ext), tested);
  ASSERT_IMAGES_EQ_EXP2(ref, tested, fixtures::ImageCompare::COMPARE_DOMAIN);
}

// BMP can read 1bpp input but not save (in OIIO), so we skip this file format
INSTANTIATE_TEST_SUITE_P(IO, OpenImageIOParameterizedTestBool, ::testing::Values("pbm", "tiff"));
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

#include <cstring>

static const auto filename = fixtures::ImagePath::concat_with_filename("test.fit");

TEST(IO, cfitsio_2D_uint8)
{
  mln::image2d<std::uint8_t> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24}  //
  };

  auto img = mln::io::fits::imread(filename, 1);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::UINT8);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<std::uint8_t, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_2D_int16)
{
  mln::image2d<std::int16_t> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24}  //
  };

  auto img = mln::io::fits::imread(filename, 2);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::INT16);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<std::int16_t, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_2D_int32)
{
  mln::image2d<std::int32_t> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24}  //
  };

  auto img = mln::io::fits::imread(filename, 3);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::INT32);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<std::int32_t, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_2D_int64)
{
  mln::image2d<std::int64_t> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24}  //
  };

  auto img = mln::io::fits::imread(filename, 4);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::INT64);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<std::int64_t, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_2D_float)
{
  mln::image2d<float> ref = {
      {0.f, 1.f, 2.f, 3.f, 4.f},      //
      {5.f, 6.f, 7.f, 8.f, 9.f},      //
      {10.f, 11.f, 12.f, 13.f, 14.f}, //
      {15.f, 16.f, 17.f, 18.f, 19.f}, //
      {20.f, 21.f, 22.f, 23.f, 24.f}  //
  };

  auto img = mln::io::fits::imread(filename, 5);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::FLOAT);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<float, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_2D_double)
{
  mln::image2d<double> ref = {
      {0.0, 1.0, 2.0, 3.0, 4.0},      //
      {5.0, 6.0, 7.0, 8.0, 9.0},      //
      {10.0, 11.0, 12.0, 13.0, 14.0}, //
      {15.0, 16.0, 17.0, 18.0, 19.0}, //
      {20.0, 21.0, 22.0, 23.0, 24.0}  //
  };

  auto img = mln::io::fits::imread(filename, 6);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::DOUBLE);
  ASSERT_EQ(img.pdim(), 2);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  auto* casted = img.cast_to<double, 2>();
  ASSERT_IMAGES_EQ_EXP(*casted, ref);
}

TEST(IO, cfitsio_3D)
{
  auto img = mln::io::fits::imread(filename, 7);
  ASSERT_EQ(img.sample_type(), mln::sample_type_id::UINT8);
  ASSERT_EQ(img.pdim(), 3);
  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 4);

  auto* casted = img.cast_to<std::uint8_t, 3>();
  int   cur    = 0;
  mln_foreach (auto p, casted->domain())
    ASSERT_EQ(casted->operator()(p), cur++);
}

TEST(IO, cfitsio_read_to_argument)
{
  mln::image2d<std::uint8_t> ref = {
      {0, 1, 2, 3, 4},      //
      {5, 6, 7, 8, 9},      //
      {10, 11, 12, 13, 14}, //
      {15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24}  //
  };

  mln::image2d<std::uint8_t> img;
  mln::io::fits::imread(filename, img, 1);

  ASSERT_EQ(img.width(), 5);
  ASSERT_EQ(img.height(), 5);

  ASSERT_IMAGES_EQ_EXP(img, ref);
}
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

static const auto filename = fixtures::ImagePath::concat_with_filename("test.fit");

TEST(IO, cfitsio_not_an_image)
{
  bool has_raised = false;
  try
  {
    auto img = mln::io::fits::imread(filename, 0);
    (void)img;
  }
  catch (std::runtime_error&)
  {
    has_raised = true;
  }
  ASSERT_TRUE(has_raised);
}

TEST(IO, cfitsio_2D_uint8)
{
  auto img = mln::io::fits::imread(filename, 1);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::UINT8);
}

TEST(IO, cfitsio_2D_int16)
{
  auto img = mln::io::fits::imread(filename, 2);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::INT16);
}

TEST(IO, cfitsio_2D_int32)
{
  auto img = mln::io::fits::imread(filename, 3);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::INT32);
}

TEST(IO, cfitsio_2D_int64)
{
  auto img = mln::io::fits::imread(filename, 4);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::INT64);
}

TEST(IO, cfitsio_2D_float)
{
  auto img = mln::io::fits::imread(filename, 5);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::FLOAT);
}

TEST(IO, cfitsio_2D_double)
{
  auto img = mln::io::fits::imread(filename, 6);
  ASSERT_TRUE(img.sample_type() == mln::sample_type_id::DOUBLE);
}
#include "segdet_tests_helper.hpp"

#include <gtest/gtest.h>

#include <mln/core/image/ndimage.hpp>
#include <scribo/segdet.hpp>


// mln::io::imsave(img, "img.tiff");
// auto img_out = mln::image2d<uint16_t>(img.width(), img.height());
// detect_line_label(img, 1, img_out);
// mln::io::imsave(img_out, "img_out.tiff");

TEST(Segdet, line_detect_one_horizontal)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 0, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;


  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_one_vertical)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;


  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_one_cross)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_4_cross)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 2, 2, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_8_cross)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 4, 4, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_15x15)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 15, 15, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_1_vert_too_long)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = segdet_output();

  auto output = detect_line(img, 101);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_1_vert_too_thick)
{
  using namespace scribo;

  auto pair = generate_test_image(100, 100, 1, 0, 15);

  mln::image2d<std::uint8_t> img = pair.first;
  segdet_output              ref = segdet_output();

  auto params          = SegDetParams();
  params.max_thickness = 10;
  auto output          = detect_line(img, 100, params);

  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

TEST(Segdet, line_detect_2_cross_noise)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 40);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(std::abs(output.angle[p]), ref.angle[p], abs_error); // absolute value to help match -89 with 90
  }
}

TEST(Segdet, line_detect_4_cross_noise)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 2, 2, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 40);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(std::abs(output.angle[p]), ref.angle[p], abs_error); // absolute value to help match -89 with 90
  }
}

TEST(Segdet, line_detect_10_vertical_20_offset)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 10, 0, 5, 20);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);


  auto abs_error = 0;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}


TEST(Segdet, line_detect_10_vertical_20_offset_noise)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 10, 0, 5, 20);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 10, 42);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(std::abs(output.angle[p]), ref.angle[p], abs_error); // absolute value to help match -89 with 90
  }
}

TEST(Segdet, line_detect_small_cross)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line(img, 10);

  auto abs_error = 0;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}

TEST(Segdet, line_detect_small_cross_noise)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}

TEST(Segdet, line_detect_one_vertical_10_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 0, 5, 0, 0, 10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}

TEST(Segdet, line_detect_one_vertical_m10_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 1, 0, 5, 0, 0, -10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}

TEST(Segdet, line_detect_one_horizontal_30_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 0, 1, 5, 0, 30, 0);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 0;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], 7);
  }
}

TEST(Segdet, line_detect_one_horizontal_m30_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 0, 1, 5, 0, -30, 0);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 2;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], 5);
  }
}

TEST(Segdet, line_detect_10_horizontal_10_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 0, 10, 5, 0, 10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 1;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], 7);
  }
}

TEST(Segdet, line_detect_cross_5_degrees)
{
  using namespace scribo;

  auto                       pair = generate_test_image(100, 100, 2, 2, 5, 10, 5, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  //  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto output = detect_line(img, 10);

  auto abs_error = 1;
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_NEAR(output.angle[p], ref.angle[p], abs_error);
  }
}

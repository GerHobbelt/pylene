#include "segdet_tests_helper.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <scribo/private/segdet_internal.hpp>
#include <scribo/segdet.hpp>


// mln::io::imsave(img, "img.tiff");
// auto img_out = mln::image2d<uint16_t>(img.width(), img.height());
// detect_line_span_label(img, 1, img_out);
// mln::io::imsave(img_out, "img_out.tiff");

using namespace scribo;
using namespace scribo::internal;

void sort_segdet_span_output(segdet_output& out)
{
  std::sort(out.seg_ids.begin(), out.seg_ids.end());
  std::sort(out.seg_ids.begin(), out.seg_ids.end());
  std::sort(out.mid_pos_x.begin(), out.mid_pos_x.end());
  std::sort(out.mid_pos_y.begin(), out.mid_pos_y.end());
  std::sort(out.thickness.begin(), out.thickness.end());
  std::sort(out.angle.begin(), out.angle.end());
}

void sort_ref_output(segdet_output& ref, segdet_output& output)
{
  sort_segdet_span_output(ref);
  sort_segdet_span_output(output);
}

void expect_empty(segdet_output output)
{
  EXPECT_EQ(output.seg_ids.size(), 0);
}

void expect_eq(segdet_output ref, segdet_output output)
{
  sort_ref_output(ref, output);

  EXPECT_EQ(output.seg_ids.size(), ref.seg_ids.size());
  EXPECT_EQ(output.seg_ids, ref.seg_ids);
  EXPECT_EQ(output.mid_pos_x, ref.mid_pos_x);
  EXPECT_EQ(output.mid_pos_y, ref.mid_pos_y);
  EXPECT_EQ(output.thickness, ref.thickness);
  EXPECT_EQ(output.angle, ref.angle);
}

void expect_near(segdet_output ref, segdet_output output, int abs_error)
{
  sort_ref_output(ref, output);

  ASSERT_EQ(output.seg_ids.size(), ref.seg_ids.size());
  for (size_t p = 0; p < output.seg_ids.size(); p++)
  {
    EXPECT_NEAR(output.seg_ids[p], ref.seg_ids[p], abs_error);
    EXPECT_NEAR(output.mid_pos_x[p], ref.mid_pos_x[p], abs_error);
    EXPECT_NEAR(output.mid_pos_y[p], ref.mid_pos_y[p], abs_error);
    EXPECT_NEAR(output.thickness[p], ref.thickness[p], abs_error);
    EXPECT_EQ(output.angle[p], ref.angle[p]);
  }
}

bool near_point(int x0, int y0, int x1, int y1, int error)
{
  int diff_x = x0 - x1;
  int diff_y = y0 - y1;

  int dist_squarred = diff_x * diff_x + diff_y * diff_y;

  return dist_squarred < error;
}

void check_vector_output(std::vector<VSegment> ref, std::vector<VSegment> output, int abs_error)
{
  ASSERT_EQ(output.size(), ref.size());
  for (auto& ref_seg : ref)
  {
    bool find = false;

    for (auto& out_seg : output)
    {
      if ((find = ((near_point(ref_seg.x0, ref_seg.y0, out_seg.x0, out_seg.y0, abs_error) &&
                    near_point(ref_seg.x1, ref_seg.y1, out_seg.x1, out_seg.y1, abs_error)) ||
                   (near_point(ref_seg.x1, ref_seg.y1, out_seg.x0, out_seg.y0, abs_error) &&
                    near_point(ref_seg.x0, ref_seg.y0, out_seg.x1, out_seg.y1, abs_error)))))
        break;
    }

    EXPECT_TRUE(find);
  }
}


TEST(Segdet, line_detect_one_horizontal)
{
  auto                       pair = generate_test_image_span(100, 100, 0, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_one_vertical)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_one_cross)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_4_cross)
{
  auto                       pair = generate_test_image_span(100, 100, 2, 2, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_8_cross)
{
  auto                       pair = generate_test_image_span(100, 100, 4, 4, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_15x15)
{
  auto                       pair = generate_test_image_span(100, 100, 15, 15, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  expect_eq(ref, output);
}

TEST(Segdet, line_detect_1_vert_too_long)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = segdet_output();

  auto output = detect_line_span(img, 101);

  expect_empty(output);
}

TEST(Segdet, line_detect_1_vert_too_thick)
{
  auto pair = generate_test_image_span(100, 100, 1, 0, 15);

  mln::image2d<std::uint8_t> img = pair.first;
  segdet_output              ref = segdet_output();

  auto params          = SegDetParams();
  params.max_thickness = 10;
  auto output          = detect_line_span(img, 100, params);

  expect_empty(output);
}

TEST(Segdet, line_detect_2_cross_noise)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 40);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  params.max_llum      = 180;
  params.max_max_llum  = 180;

  auto output = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_4_cross_noise)
{
  auto                       pair = generate_test_image_span(100, 100, 2, 2, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 40);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_10_vertical_20_offset)
{
  auto                       pair = generate_test_image_span(100, 100, 10, 0, 5, 20);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 0;
  expect_near(ref, output, abs_error);
}


TEST(Segdet, line_detect_10_vertical_20_offset_noise)
{
  auto                       pair = generate_test_image_span(100, 100, 10, 0, 5, 20);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 10, 42);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_small_cross)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 0;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_small_cross_noise)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_small_cross_noise_leplumey)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  params.tracker       = SEGDET_PROCESS_TRACKING_ENUM::KALMAN_LEPLUMEY;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_small_cross_noise_sma)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  params.tracker       = SEGDET_PROCESS_TRACKING_ENUM::SIMPLE_MOVING_AVERAGE;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_span(img, 10, params);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_one_vertical_10_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5, 0, 0, 10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_one_vertical_m10_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5, 0, 0, -10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_one_horizontal_30_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 0, 1, 5, 0, 30, 0);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 0;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_one_horizontal_m30_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 0, 1, 5, 0, -30, 0);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 2;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_10_horizontal_10_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 0, 10, 5, 0, 10);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 1;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_cross_5_degrees)
{
  auto                       pair = generate_test_image_span(100, 100, 2, 2, 5, 10, 5, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto output = detect_line_span(img, 10);

  auto abs_error = 1;
  expect_near(ref, output, abs_error);
}

TEST(Segdet, line_detect_cross_5_degrees_vector)
{
  auto                          pair = generate_test_image_vector(100, 100, 2, 2, 5, 10, 5, 5);
  mln::image2d<std::uint8_t>    img  = pair.first;
  std::vector<scribo::VSegment> ref  = pair.second;

  auto output = detect_line_vector(img, 10);

  auto abs_error = 1;
  check_vector_output(ref, output, abs_error);
}

TEST(Segdet, line_detect_one_vertical_m10_degrees_vector)
{
  auto pair = generate_test_image_vector(100, 100, 1, 0, 5, 0, 0, -10);

  mln::image2d<std::uint8_t>    img = pair.first;
  std::vector<scribo::VSegment> ref = pair.second;

  auto output = detect_line_vector(img, 10);

  auto abs_error = 1;
  check_vector_output(ref, output, abs_error);
}

TEST(Segdet, line_detect_small_cross_noise_vector)
{
  auto                       pair = generate_test_image_vector(100, 100, 1, 1, 5, 30);
  mln::image2d<std::uint8_t> img  = pair.first;
  auto                       ref  = pair.second;
  add_gaussian_noise_on_image(img, 100, 30, 4);

  auto params          = SegDetParams();
  params.preprocess    = SEGDET_PREPROCESS_ENUM::BLACK_TOP_HAT;
  int max_dim          = std::max(img.height(), img.width());
  params.max_thickness = static_cast<int>(std::ceil(max_dim * 0.4));
  auto output          = detect_line_vector(img, 10, params);

  auto abs_error = 5;
  check_vector_output(ref, output, abs_error);
}

TEST(Segdet, bucket_size_dividor)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto params        = SegDetParams();
  params.bucket_size = 100 / 2;

  auto output = detect_line_span(img, 10, params);

  expect_eq(ref, output);
}

TEST(Segdet, bucket_size_higher_one_bucket)
{
  auto                       pair = generate_test_image_span(100, 100, 1, 0, 5);
  mln::image2d<std::uint8_t> img  = pair.first;
  segdet_output              ref  = pair.second;

  auto params        = SegDetParams();
  params.bucket_size = 100 * 2;

  auto output = detect_line_span(img, 10, params);

  expect_eq(ref, output);
}

TEST(Segdet, odd_observation_on_odd)
{
  mln::image2d<std::uint8_t> img = {{255, 0, 255}, {255, 0, 255}, {255, 0, 255}, {255, 0, 255}, {255, 0, 255}};
  mln::image2d<std::uint8_t> ref = {{0, 2, 0}};

  int  min_len          = 0;
  auto params           = SegDetParams();
  params.traversal_mode = SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL;

  auto [out, supperpositon] = detect_line_label(img, min_len, params);

  for (int y = 0; y < img.height(); y++)
    for (int x = 0; x < img.width(); x++)
    {
      EXPECT_EQ(static_cast<int>(ref({x, 0})), static_cast<int>(out({x, y})));
    }
}

TEST(Segdet, even_observation_on_odd)
{
  mln::image2d<std::uint8_t> img = {
      {255, 0, 0, 255}, {255, 0, 0, 255}, {255, 0, 0, 255}, {255, 0, 0, 255}, {255, 0, 0, 255}};
  mln::image2d<std::uint8_t> ref = {{0, 2, 2, 0}};

  int  min_len          = 0;
  auto params           = SegDetParams();
  params.traversal_mode = SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL;

  auto [out, supperpositon] = detect_line_label(img, min_len, params);

  for (int y = 0; y < img.height(); y++)
    for (int x = 0; x < img.width(); x++)
    {
      EXPECT_EQ(static_cast<int>(ref({x, 0})), static_cast<int>(out({x, y})));
    }
}

TEST(Segdet, even_observation_on_even)
{
  mln::image2d<std::uint8_t> img = {{255, 255, 0, 0, 255},
                                    {255, 255, 0, 0, 255},
                                    {255, 255, 0, 0, 255},
                                    {255, 255, 0, 0, 255},
                                    {255, 255, 0, 0, 255}};
  mln::image2d<std::uint8_t> ref = {{0, 0, 2, 2, 0}};

  int  min_len          = 0;
  auto params           = SegDetParams();
  params.traversal_mode = SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL;

  auto [out, supperpositon] = detect_line_label(img, min_len, params);

  for (int y = 0; y < img.height(); y++)
    for (int x = 0; x < img.width(); x++)
    {
      EXPECT_EQ(static_cast<int>(ref({x, 0})), static_cast<int>(out({x, y})));
    }
}

TEST(Segdet, odd_observation_on_even)
{
  mln::image2d<std::uint8_t> img = {
      {255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}};
  mln::image2d<std::uint8_t> ref = {{0, 0, 2, 0}};

  int  min_len          = 0;
  auto params           = SegDetParams();
  params.traversal_mode = SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL;

  auto [out, supperpositon] = detect_line_label(img, min_len, params);

  for (int y = 0; y < img.height(); y++)
    for (int x = 0; x < img.width(); x++)
    {
      EXPECT_EQ(static_cast<int>(ref({x, 0})), static_cast<int>(out({x, y})));
    }
}
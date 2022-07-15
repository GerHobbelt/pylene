#include "segdet_tests_helper.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndimage.hpp>

#include <random>
#include <tuple>

void draw_line(mln::image2d<std::uint8_t> image, scribo::internal::segdet_output& ref,
               std::vector<scribo::VSegment>& ref_vsegment, int start_index, int thickness, bool horizontal, int seg_id,
               int offset, int angle)
{
  int max_dim   = horizontal ? image.width() : image.height();
  int max_dim_v = horizontal ? image.height() : image.width();

  bool first = true;
  int  x0    = -1;
  int  y0    = -1;
  int  x1    = -1;
  int  y1    = -1;


  float slope = std::tan(-angle * 3.14f / 180.0f);
  for (int i = offset; i < max_dim - offset; i++)
  {
    int current_index = start_index + int(static_cast<float>(i - offset) * slope);

    if (current_index + thickness / 2 >= max_dim_v || current_index - thickness / 2 < 0)
      break;

    int actual_thickness = 0;
    for (int j = -thickness / 2; j < thickness / 2 + 1; j++)
    {
      if (current_index + j < max_dim_v && current_index + j >= 0)
      {
        if (horizontal)
          image({i, current_index + j}) = 0;
        else
          image({current_index + j, i}) = 0;
        actual_thickness++;
      }
    }

    if (actual_thickness != thickness)
    {
      if (current_index + thickness / 2 >= max_dim_v)
        current_index = max_dim_v - actual_thickness / 2;
      else
        current_index = actual_thickness / 2;
    }

    ref.seg_ids.push_back(seg_id);
    ref.angle.push_back(horizontal);

    if (horizontal)
    {
      ref.mid_pos_x.push_back(static_cast<float>(i));
      ref.mid_pos_y.push_back(static_cast<float>(current_index));
    }
    else
    {
      ref.mid_pos_x.push_back(static_cast<float>(current_index));
      ref.mid_pos_y.push_back(static_cast<float>(i));
    }

    ref.thickness.push_back(actual_thickness);

    if (first)
    {
      x0 = ref.mid_pos_x[ref.mid_pos_x.size() - 1];
      y0 = ref.mid_pos_y[ref.mid_pos_y.size() - 1];

      first = false;
    }
  }

  x1 = ref.mid_pos_x[ref.mid_pos_x.size() - 1];
  y1 = ref.mid_pos_y[ref.mid_pos_y.size() - 1];

  scribo::VSegment vseg = {
    .label = -1,
    .x0 = x0,
    .y0 = y0,
    .x1 = x1,
    .y1 = y1
  };
  ref_vsegment.push_back(vseg);
}

std::tuple<mln::image2d<std::uint8_t>, scribo::internal::segdet_output, std::vector<scribo::VSegment>>
generate_test_image(int height, int width, int nb_vertical, int nb_horizontal, int thickness, int offset, int angle_h,
                    int angle_v)
{
  using namespace mln;
  using namespace scribo;
  using namespace scribo::internal;

  if (nb_vertical > width / thickness || nb_horizontal > height / thickness)
    throw std::invalid_argument("The number of vertical segments cannot be greater than the image dimensions.");
  if (angle_h > 45 || angle_v > 45 || angle_v < -45 || angle_h < -45)
    throw std::invalid_argument("The angle must be between -45 and 45 degrees.");

  // Create new image and fill with white
  auto                          out_image        = mln::image2d<std::uint8_t>(width, height);
  auto                          reference_output = segdet_output();
  std::vector<scribo::VSegment> ref_vsegment;

  mln::fill(out_image, 255);

  for (int i = 0; i < nb_horizontal; i++)
  {
    draw_line(out_image, reference_output, ref_vsegment, height / (nb_horizontal + 1) * (i + 1), thickness, true, i,
              offset, angle_h);
  }

  for (int i = 0; i < nb_vertical; i++)
  {
    draw_line(out_image, reference_output, ref_vsegment, width / (nb_vertical + 1) * (i + 1), thickness, false,
              i + nb_horizontal, offset, angle_v);
  }

  return std::make_tuple(out_image, reference_output, ref_vsegment);
}

std::pair<mln::image2d<std::uint8_t>, scribo::internal::segdet_output>
generate_test_image_span(int height, int width, int nb_vertical, int nb_horizontal, int thickness, int offset,
                         int angle_h, int angle_v)
{
  auto [img, segdet_output, vsgements] =
      generate_test_image(height, width, nb_vertical, nb_horizontal, thickness, offset, angle_h, angle_v);
  return std::make_pair(img, segdet_output);
}

std::pair<mln::image2d<std::uint8_t>, std::vector<scribo::VSegment>>
generate_test_image_vector(int height, int width, int nb_vertical, int nb_horizontal, int thickness, int offset,
                           int angle_h, int angle_v)
{
  auto [img, segdet_output, vsgements] =
      generate_test_image(height, width, nb_vertical, nb_horizontal, thickness, offset, angle_h, angle_v);
  return std::make_pair(img, vsgements);
}

void add_gaussian_noise_on_image(mln::image2d<std::uint8_t> input, int mean, int std, int seed)
{
  std::default_random_engine engine(seed);
  auto                       dist = std::normal_distribution<float>(mean, std);
  for (int i = 0; i < input.height(); i++)
  {
    for (int j = 0; j < input.width(); j++)
    {
      int noise = int(dist(engine));
      if (int(input({i, j})) - noise >= 0)
        input({i, j}) -= noise;
    }
  }
}
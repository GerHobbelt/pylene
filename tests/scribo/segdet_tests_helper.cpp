#include "segdet_tests_helper.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndimage.hpp>

#include <random>

void draw_line(mln::image2d<std::uint8_t> image, scribo::segdet_output& ref, int start_index,
               int thickness, bool horizontal, int seg_id, int offset, int angle)
{
  int max_dim = horizontal ? image.width() : image.height();
  int max_dim_v = horizontal ? image.height() : image.width();

  float slope = std::tan(-angle * 3.14f / 180.0f);
  for (int i = offset; i < max_dim - offset; i++)
  {
    ref.seg_ids.push_back(seg_id);

    int current_index = start_index + int( static_cast<float>(i - offset) * slope);

    if (current_index + thickness / 2 >= max_dim_v || current_index - thickness / 2 < 0)
    {
      //      break; // We are out of the image and we should stop here
      if (current_index - thickness / 2 >= max_dim_v || current_index + thickness / 2 < 0)
        break;
    }

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

    if (horizontal)
    {
      ref.mid_pos_x.push_back(static_cast<float>(i));
      ref.mid_pos_y.push_back(static_cast<float>(current_index));
      ref.angle.push_back(angle);
    }
    else
    {
      ref.mid_pos_x.push_back(static_cast<float>(current_index));
      ref.mid_pos_y.push_back(static_cast<float>(i));
      ref.angle.push_back(((angle >= 0) - (angle < 0)) * (90 - std::abs(angle)));
    }

    ref.thickness.push_back(actual_thickness);

  }
}

std::pair<mln::image2d<std::uint8_t>, scribo::segdet_output>
generate_test_image(int height, int width, int nb_vertical, int nb_horizontal, int thickness, int offset, int angle_h, int angle_v)
{
  using namespace mln;
  using namespace scribo;

  if (nb_vertical > width / thickness || nb_horizontal > height / thickness)
    throw std::invalid_argument("The number of vertical segments cannot be greater than the image dimensions.");
  if (angle_h > 45 || angle_v > 45 || angle_v < -45 || angle_h < -45)
    throw std::invalid_argument("The angle must be between -45 and 45 degrees.");

  // Create new image and fill with white
  auto out_image        = mln::image2d<std::uint8_t>(width, height);
  auto reference_output = segdet_output();

  mln::fill(out_image, 255);

  for (int i = 0; i < nb_horizontal; i++)
  {
    draw_line(out_image, reference_output, height / (nb_horizontal + 1) * (i + 1), thickness, true, i, offset, angle_h);
  }
  for (int i = 0; i < nb_vertical; i++)
  {
    draw_line(out_image, reference_output, width / (nb_vertical + 1) * (i + 1), thickness, false, i + nb_horizontal,
              offset, angle_v);
  }

  return std::make_pair(out_image, reference_output);
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
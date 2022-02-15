#pragma once

#include <mln/core/image/image.hpp>
#include <scribo/segdet.hpp>

std::pair<mln::image2d<std::uint8_t>, scribo::segdet_output>
generate_test_image(int height, int width, int nb_vertical, int nb_horizontal, int thickness = 2, int offset = 0, int angle_h = 0, int angle_v = 0);

void add_gaussian_noise_on_image(mln::image2d<std::uint8_t> input, int mean, int std, int seed = 42);

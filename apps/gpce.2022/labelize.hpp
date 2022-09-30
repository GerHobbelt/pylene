#pragma once

#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>

#include <tuple>

std::tuple<mln::image2d<uint8_t>, int> labelize_serial(const mln::image2d<mln::rgb8>& input);
std::tuple<mln::image2d<uint8_t>, int> labelize_view(const mln::image2d<mln::rgb8>& input);

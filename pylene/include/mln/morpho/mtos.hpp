#pragma once

#include <mln/core/colors.hpp>
#include <mln/morpho/component_tree.hpp>

#include <tuple>

namespace mln::morpho
{
  std::pair<component_tree<>, image2d<int>> mtos(image2d<rgb8> ima, point2d pstart);
}
#pragma once

#include <mln/morpho/component_tree.hpp>

#include <tuple>

namespace mln::morpho::details
{
  std::pair<component_tree<>, image2d<int>> satmaxtree(image2d<std::uint16_t> depth_map);
}
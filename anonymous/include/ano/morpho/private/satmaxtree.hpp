#pragma once

#include <ano/morpho/component_tree.hpp>

#include <tuple>

namespace ano::morpho::details
{
  /// \brief Compute the hole-filled maxtree of a depth map
  /// \param depth_map The input depth map
  /// \param pstart The rooting point of the satmaxtree
  /// \return A pair (tree, nodemap) where tree is the resulting satmaxtree (values are not kept)
  ///         and a map image point -> tree node
  std::pair<component_tree<>, image2d<int>> satmaxtree(image2d<std::uint16_t> depth_map, point2d pstart);
} // namespace ano::morpho::details
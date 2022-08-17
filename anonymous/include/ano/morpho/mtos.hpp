#pragma once

#include <ano/core/colors.hpp>
#include <ano/morpho/component_tree.hpp>

#include <tuple>

namespace ano::morpho
{
  /// \brief Compute the multivariate tree of shapes
  /// \param ima The input image
  /// \param pstart The rooting point
  /// \return A pair (tree, nodemap)
  std::pair<component_tree<>, image2d<int>> mtos(image2d<rgb8> ima, point2d pstart);
} // namespace ano::morpho
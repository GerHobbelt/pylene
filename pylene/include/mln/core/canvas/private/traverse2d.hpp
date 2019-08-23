#pragma once

#include <mln/core/box.hpp>

#include <functional>


namespace mln::canvas::details
{

  /// \brief Traverse a box2d following a given direction, and call a function foreach line
  /// callback(src, direction, size)
  void traverse_along_direction(mln::experimental::box2d roi,
                                mln::experimental::point2d direction,
                                std::function<void(mln::experimental::point2d, mln::experimental::point2d, std::size_t n)> callback);
}

#pragma once

#include <ano/core/box.hpp>
#include <ano/core/image/ndimage_fwd.hpp>
#include <functional>


namespace ano::canvas::details
{

  /// \brief Traverse a box2d following a given direction, and call a function foreach line
  /// callback(src, direction, size)
  void traverse_along_direction(ano::box2d roi, ano::point2d direction,
                                std::function<void(ano::point2d, ano::point2d, std::size_t n)> callback);

  /// \brief Traverse an image and calls f with a pointer to the data of each line
  void apply_line(ano::ndbuffer_image& input, std::function<void(std::byte*)>);
} // namespace ano::canvas::details

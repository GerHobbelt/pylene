#pragma once

#include <mln/core/experimental/point.hpp>
#include <vector>


namespace mln
{


  /// Compute the convexhull of a set of points
  /// \param points
  /// \pre \p points must be sorted along first dimension, then second dim.
  ///         (scan order)
  std::vector<experimental::point2d> convexhull(const std::vector<experimental::point2d>& points);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    std::vector<experimental::point2d> cvxhull(const experimental::point2d* points, std::size_t n);
  }

  std::vector<experimental::point2d> convexhull(const std::vector<experimental::point2d>& points)
  {
    return impl::cvxhull(points.data(), points.size());
  }
} // namespace mln

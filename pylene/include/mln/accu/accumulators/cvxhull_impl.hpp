#pragma once

#include <mln/core/point.hpp>
#include <vector>


namespace mln
{

  /// Compute the convexhull of a set of points. Note that the input array in modified inplace (sorted)
  /// \param points
  std::vector<point2d> convexhull(mln::point2d* points, std::size_t n);



  /// Compute the convexhull of a set of points
  /// \param points
  /// \pre \p points must be sorted along first dimension, then second dim.
  ///         (scan order)
  std::vector<point2d> convexhull_sorted(const std::vector<point2d>& points);



  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {
    std::vector<point2d> cvxhull_sorted(const point2d* points, std::size_t n);
  }

} // namespace mln

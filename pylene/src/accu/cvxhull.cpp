#include <mln/accu/accumulators/cvxhull_impl.hpp>

#include <mln/core/point.hpp>
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <vector>


namespace
{

  /// \brief Compute angles from a reference point and return the position of the minimum element
  ///
  /// \param ref reference point
  /// \param points Array of points
  /// \param angles Array of angles (computed)
  /// \param n Number of points
  /// \param backward True if going backward (y decreasing), false otherwise
  /// \return The position of the minimum
  std::size_t get_min_angles(mln::point2d        ref,    //
                             const mln::point2d* points, //
                             float*                            angles, //
                             std::size_t                       n,      //
                             bool                              backward)
  {
    // Compute the angles to the reference points: atan( (x - refx) / (y - refy) )
    // Because tan is increasing and we just need the max, we just compute (x - refx) / (y - refy)
    // Ensure that when computing dx/dy, dy is always dy >= 0 (for the sign of infinity when dy=0)

    if (backward)
      std::transform(points, points + n, angles, [ref](auto p) { return float(ref.x() - p.x()) / (ref.y() - p.y()); });
    else
      std::transform(points, points + n, angles, [ref](auto p) { return float(p.x() - ref.x()) / (p.y() - ref.y()); });


    // FIXME: use soft (eps) float comparison to avoid multiple colinear segments
    // Get the position of the last maximum i

    if (backward)
    {
      return std::distance(angles, std::min_element(angles, angles + n));
    }
    else
    {
      auto a = std::make_reverse_iterator(angles + n);
      auto b = std::make_reverse_iterator(angles);
      return std::distance(std::min_element(a, b), b) - 1;
    }
  }
} // namespace


namespace mln
{
  namespace impl
  {
    std::vector<point2d> cvxhull_sorted(const point2d* points, std::size_t n)
    {
      if (n == 0)
        return {};


      std::vector<point2d> cvx_hull;
      std::vector<float>                 angles(n);


      std::size_t pos = 0;
      cvx_hull.push_back(points[pos]);

      // Go Down
      while (n - pos > 1)
      {
        pos += 1 + get_min_angles(points[pos], points + pos + 1, angles.data() + pos + 1, n - pos - 1, false);
        if (!std::isnan(angles[pos])) // Handle duplicate points
          cvx_hull.push_back(points[pos]);
      }

      assert(pos == n - 1);

      // Go up
      // Pos is past-the end
      while (pos > 0)
      {
        pos = get_min_angles(points[pos], points, angles.data(), pos, true);
        if (pos == 0)
          break;

        if (std::isfinite(angles[pos]))
          cvx_hull.push_back(points[pos]);
      }

      return cvx_hull;
    }
  } // namespace impl


  std::vector<point2d> convexhull(point2d* points, std::size_t n)
  {
    std::sort(points, points + n);
    return impl::cvxhull_sorted(points, n);
  }

  std::vector<point2d> convexhull_sorted(const std::vector<point2d>& points)
  {
    return impl::cvxhull_sorted(points.data(), points.size());
  }

} // namespace mln

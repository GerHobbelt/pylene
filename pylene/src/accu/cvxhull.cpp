#include <mln/accu/accumulators/cvxhull_impl.hpp>

#include <mln/core/experimental/point.hpp>
#include <cstddef>
#include <vector>


namespace
{

  /// \brief Compute angles from a reference point and return the position of the minimum element
  ///
  /// \param ref reference point
  /// \param points Array of points
  /// \param angles Array of angles (computed)
  /// \param n Number of points
  /// \param first_minimum (if true, return the first minimum in the range, else the last)
  /// \return an iterator of the output array that points to its maximum
  std::size_t get_min_angles(mln::experimental::point2d        ref,    //
                             const mln::experimental::point2d* points, //
                             float*                            angles, //
                             std::size_t                       n,      //
                             bool                              first_minimum = true)
  {
    // Compute the angles to the reference points: atan( (x - refx) / (y - refy) )
    // Because tan is increasing and we just need the max, we just compute (x - refx) / (y - refy)
    std::transform(points, points + n, angles, [ref](auto p) { return float(p.x() - ref.x()) / (p.y() - ref.y()); });


    // FIXME: use soft (eps) float comparison to avoid multiple colinear segments
    // Get the position of the last maximum i

    if (first_minimum)
    {
      return std::min_element(angles, angles + n) - angles;

    }
    else
    {
      auto a = std::make_reverse_iterator(angles + n);
      auto b = std::make_reverse_iterator(angles);
      return std::min_element(a, b) - a;
    }
  }
} // namespace


namespace mln::impl
{

  std::vector<experimental::point2d>
  cvxhull(const experimental::point2d* points, std::size_t n)
  {
    if (n == 0)
      return {};


    std::vector<experimental::point2d> cvx_hull;
    std::vector<float>                 angles(n);



    std::size_t pos = 0;
    cvx_hull.push_back(points[pos]);

    // Go Down
    while (n - pos > 1)
    {
      pos += get_min_angles(points[pos], points + pos + 1, angles.data() + pos + 1, n - pos - 1, true);
      cvx_hull.push_back(points[pos]);
    }


    if (pos != n - 1)
    {
      pos = n - 1;
      cvx_hull.push_back(points[pos]);
    }


    // Go up
    // Pos is past-the end
    while (pos > 0)
    {
      pos = get_min_angles(points[pos], points, angles.data(), pos, false);
      cvx_hull.push_back(points[pos]);
    }

    return cvx_hull;
  }
}

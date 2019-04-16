#include <mln/core/se/mask2d.hpp>
#include <exception>

namespace
{
  std::vector<mln::point2d>
  initlist_to_vector(const std::initializer_list<std::initializer_list<int>>& values, int& width, int& height)
  {
    std::vector<mln::point2d> vals;

    height = values.size();
    if (height == 0)
      throw std::runtime_error("Invalid height for mask2d");

    if (height % 2 == 0)
      throw std::runtime_error("Invalid height for mask2d (must be odd)");

    auto row = values.begin();

    width = row->size();
    if (width == 0)
      throw std::runtime_error("Invalid width for mask2d");

    if (width % 2 == 0)
      throw std::runtime_error("Invalid width for mask2d (must be odd)");

    vals.reserve(width * height);

    for (int y = -(height/2); y <= (height/2); ++y, ++row)
    {
      if (static_cast<int>(row->size()) != width)
        throw std::runtime_error("A line has an invalid width");

      auto val = row->begin();
      for (int x = -(width/2); x <= (width/2); ++x, ++val)
        if (*val)
          vals.push_back(mln::point2d{static_cast<short int>(y), static_cast<short int>(x)});
    }
    return vals;
  }
}


namespace mln::se
{
  mask2d::mask2d(std::initializer_list<std::initializer_list<int>> values)
  {
    int width, height;
    m_points = initlist_to_vector(values, width, height);
    m_radial_extent = std::max(width, height) / 2;
  }

  namespace experimental
  {

    mask2d::mask2d(std::initializer_list<std::initializer_list<int>> values)
    {
      int width, height;
      m_points        = initlist_to_vector(values, width, height);
      m_radial_extent = std::max(width, height) / 2;
    }

    ::ranges::span<const mln::point2d> mask2d::offsets() const
    {
      return ::ranges::make_span(m_points.data(), m_points.size());
    }

  } // namespace experimental
} // namespace mln

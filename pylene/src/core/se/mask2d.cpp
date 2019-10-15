#include <mln/core/se/mask2d.hpp>

#include <cstring>
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

  std::vector<mln::experimental::point2d>
  initlist_to_vector_2(const std::initializer_list<std::initializer_list<int>>& values, int& width, int& height)
  {
    std::vector<mln::experimental::point2d> vals;

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
          vals.push_back({x,y});
    }
    return vals;
  }


  // Call a function foreach non-zero value of  an initialization list
  struct mask_info
  {
    int width;
    int height;
    int before_size;
    int after_size;
  };

  mask_info initlist_to_wvector(std::initializer_list<std::initializer_list<std::byte>> values,
                                void* zero,
                                std::size_t sample_size,
                                std::function<void(int x, int y, void*)> callback)
  {
    int height = values.size();
    if (height == 0)
      throw std::runtime_error("Invalid height for mask2d");

    if (height % 2 == 0)
      throw std::runtime_error("Invalid height for mask2d (must be odd)");

    auto row = values.begin();

    int width = row->size();
    if (width == 0)
      throw std::runtime_error("Invalid width for mask2d");

    if (width % 2 == 0)
      throw std::runtime_error("Invalid width for mask2d (must be odd)");

    int before = 0;
    int after = 0;
    for (int y = -(height / 2); y <= (height / 2); ++y, ++row)
    {
      if (static_cast<int>(row->size()) != width)
        throw std::runtime_error("A line has an invalid width");

      const std::byte* ptr = row->begin();
      for (int x = -(width / 2); x <= (width / 2); ++x)
      {
        if (std::memcmp((void*)ptr, zero, sample_size) != 0)
        {
          if (y < 0 || (y == 0 && x < 0))
            before++;
          else if (y > 0 || (y == 0 && x > 0))
            after++;
          callback(x, y, (void*)ptr);
        }
        ptr += sample_size;
      }
    }

    mask_info info = {width, height, before, after};
    return info;
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
      m_points        = initlist_to_vector_2(values, width, height);
      m_radial_extent = std::max(width, height) / 2;
    }

    void wmask2d_base::init(void* values, void* zero, std::size_t sample_size, std::function<void(int x, int y, void*)> callback)
    {
      auto* vals = reinterpret_cast<std::initializer_list<std::initializer_list<std::byte>>*>(values);
      auto info = initlist_to_wvector(*vals, zero, sample_size, std::move(callback));

      m_radial_extent = std::max(info.width, info.height) / 2;
      m_before_size = info.before_size;
      m_after_size = info.after_size;
    }

    ::ranges::span<const mln::experimental::point2d> mask2d::offsets() const
    {
      return ::ranges::make_span(m_points.data(), m_points.size());
    }


  } // namespace experimental
} // namespace mln::se

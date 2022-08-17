#include <ano/core/se/mask2d.hpp>

#include <cstring>
#include <exception>
#include <fmt/core.h>

namespace
{
  std::vector<ano::point2d> initlist_to_vector(const std::initializer_list<std::initializer_list<int>>& values,
                                               int& width, int& height)
  {
    std::vector<ano::point2d> vals;

    height = static_cast<int>(values.size());
    if (height == 0 || height % 2 == 0)
      throw std::runtime_error(fmt::format("Invalid height ({}) for mask2d (must be odd and non-null)", height));

    auto row = values.begin();

    width = static_cast<int>(row->size());
    if (width == 0 || width % 2 == 0)
      throw std::runtime_error(fmt::format("Invalid width ({}) for mask2d (must be odd and non-null)", width));

    vals.reserve(width * height);

    for (int y = -(height / 2); y <= (height / 2); ++y, ++row)
    {
      if (static_cast<int>(row->size()) != width)
        throw std::runtime_error(
            fmt::format("The line {} has an invalid width ({} should be {})", y + height / 2, row->size(), width));

      auto val = row->begin();
      for (int x = -(width / 2); x <= (width / 2); ++x, ++val)
        if (*val)
          vals.push_back({x, y});
    }
    return vals;
  }


  // Call a function foreach non-zero value of  an initialization list
  struct mask_info
  {
    int before_size;
    int after_size;
  };

  mask_info initlist_to_wvector(std::initializer_list<std::initializer_list<std::byte>> values, void* zero, int* sizes,
                                std::size_t sample_size, std::function<void(int x, int y, void*)> callback)
  {
    int width  = sizes[0];
    int height = sizes[1];

    if (height == 0 || height % 2 == 0)
      throw std::runtime_error(fmt::format("Invalid height ({}) for mask2d (must be odd and non-null)", height));

    if (width == 0 || width % 2 == 0)
      throw std::runtime_error(fmt::format("Invalid width ({}) for mask2d (must be odd and non-null)", width));

    int         before  = 0;
    int         after   = 0;
    auto        row     = values.begin();
    std::size_t rowsize = row->size();

    for (int y = -(height / 2); y <= (height / 2); ++y, ++row)
    {
      if (row->size() != rowsize)
      {
        throw std::runtime_error(
            fmt::format("The line {} has an invalid width ({} should be {})", y + height / 2, row->size(), rowsize));
      }

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

    mask_info info = {before, after};
    return info;
  }
} // namespace


namespace ano::se
{

  namespace details
  {
    void wmask2d_base::init(void* values, void* zero, int* sizes, std::size_t sample_size,
                            std::function<void(int x, int y, void*)> callback)
    {
      auto* vals = reinterpret_cast<std::initializer_list<std::initializer_list<std::byte>>*>(values);
      auto  info = initlist_to_wvector(*vals, zero, sizes, sample_size, std::move(callback));

      m_radial_extent = std::max(sizes[0], sizes[1]) / 2;
      m_before_size   = info.before_size;
      m_after_size    = info.after_size;
    }
  } // namespace details


  mask2d::mask2d(std::initializer_list<std::initializer_list<int>> values)
  {
    int width, height;
    m_points        = initlist_to_vector(values, width, height);
    m_radial_extent = std::max(width, height) / 2;
  }


  ::ranges::span<const ano::point2d> mask2d::offsets() const
  {
    return ::ranges::make_span(m_points.data(), m_points.size());
  }


  ano::box2d mask2d::compute_input_region(ano::box2d roi) const noexcept
  {
    roi.inflate(m_radial_extent);
    return roi;
  }


  ano::box2d mask2d::compute_output_region(ano::box2d roi) const noexcept
  {
    roi.inflate(-m_radial_extent);
    return roi;
  }

} // namespace ano::se

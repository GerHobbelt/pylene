#pragma once

#include <ano/core/box.hpp>
#include <ano/core/neighborhood/private/neighborhood_facade.hpp>
#include <ano/core/point.hpp>

#include <range/v3/view/span.hpp>

namespace ano
{
  // Forward
  struct c4_t;
  struct c8_t;

  enum class c2d_type : std::ptrdiff_t
  {
    C4 = 4,
    C8 = 8
  };

  struct dyn_nbh_2d_t : neighborhood_facade<dyn_nbh_2d_t>
  {
  private:
    using point_t = ano::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    constexpr dyn_nbh_2d_t(c2d_type t)
      : m_type(t)
    {
    }

    constexpr dyn_nbh_2d_t(const ano::c4_t&)
      : m_type(c2d_type::C4)
    {
    }

    constexpr dyn_nbh_2d_t(const ano::c8_t&)
      : m_type(c2d_type::C8)
    {
    }

    constexpr auto offsets() const
    {
      return ::ranges::span<const point_t>{m_offsets.data(), static_cast<std::ptrdiff_t>(m_type)};
    }

    // Create span with before and after offsets to reduce compile time
    // Return a span
    constexpr auto before_offsets() const
    {
      return ::ranges::span<const point_t>{m_before_offsets.data(), static_cast<std::ptrdiff_t>(m_type) / 2};
    }

    constexpr auto after_offsets() const
    {
      return ::ranges::span<const point_t>{m_after_offsets.data(), static_cast<std::ptrdiff_t>(m_type) / 2};
    }

    static constexpr int radial_extent() { return 1; }

    /// \brief Return the input ROI for 2D box.
    ano::box2d compute_input_region(ano::box2d roi) const
    {
      roi.inflate(1);
      return roi;
    }

    /// \brief Return the output ROI for 2D box.
    ano::box2d compute_output_region(ano::box2d roi) const
    {
      roi.inflate(-1);
      return roi;
    }

    constexpr auto alternative() const { return dyn_nbh_2d_t(m_type == c2d_type::C4 ? c2d_type::C8 : c2d_type::C4); }

  private:
    const c2d_type m_type;
    // clang-format off
    static inline constexpr std::array<point_t, 8> m_offsets = {{
        {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}
    }};
    static inline constexpr std::array<point_t, 4> m_before_offsets = {{
      {-1, 0}, {0, -1}, {-1, -1}, {1, -1}
    }};
    static inline constexpr std::array<point_t, 4> m_after_offsets = {{
      {1, 0}, {0, 1}, {1, 1}, {-1, 1}
    }};
    // clang-format on
  };
} // namespace ano
#pragma once

#include <ano/core/neighborhood/private/neighborhood_facade.hpp>
#include <ano/core/point.hpp>

#include <array>
#include <range/v3/view/span.hpp>

namespace ano
{
  struct c26_t : neighborhood_facade<c26_t>
  {
  private:
    using point_t = ano::ndpoint<3, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 26> offsets() { return {m_offsets.data(), 26}; }
    static constexpr ::ranges::span<const point_t, 13> before_offsets() { return {m_offsets.data(), 13}; }
    static constexpr ::ranges::span<const point_t, 13> after_offsets() { return {m_offsets.data() + 13, 13}; }

    static constexpr int radial_extent() { return 1; }

    /// \brief Return the input ROI for 3D box.
    ano::box3d compute_input_region(ano::box3d roi) const
    {
      roi.inflate(1);
      return roi;
    }

    /// \brief Return the output ROI for 3D box.
    ano::box3d compute_output_region(ano::box3d roi) const
    {
      roi.inflate(-1);
      return roi;
    }


  private:
    static inline constexpr std::array<point_t, 26> m_offsets = {{
        // clang-format off
        {-1, -1, -1}, {+0, -1, -1}, {+1, -1, -1},
        {-1, +0, -1}, {+0, +0, -1}, {+1, +0, -1},
        {-1, +1, -1}, {+0, +1, -1}, {+1, +1, -1},
        // ----
        {-1, -1, +0}, {+0, -1, +0}, {+1, -1, +0},
        {-1, +0, +0},               {+1, +0, +0},
        {-1, +1, +0}, {+0, +1, +0}, {+1, +1, +0},
        // ----
        {-1, -1, +1}, {+0, -1, +1}, {+1, -1, +1},
        {-1, +0, +1}, {+0, +0, +1}, {+1, +0, +1},
        {-1, +1, +1}, {+0, +1, +1}, {+1, +1, +1},
        // clang-format on
    }};
  };

  static constexpr inline c26_t c26 = {};
} // namespace ano

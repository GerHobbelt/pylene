#pragma once

#include <ano/core/box.hpp>
#include <ano/core/neighborhood/private/neighborhood_facade.hpp>
#include <ano/core/point.hpp>

#include <array>
#include <range/v3/view/span.hpp>

namespace ano
{
  struct c4_t : neighborhood_facade<c4_t>
  {
  private:
    using point_t = ano::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 4> offsets() { return {m_offsets.data(), 4}; }
    static constexpr ::ranges::span<const point_t, 2> before_offsets() { return {m_offsets.data(), 2}; }
    static constexpr ::ranges::span<const point_t, 2> after_offsets() { return {m_offsets.data() + 2, 2}; }

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


  private:
    // clang-format off
    static inline constexpr std::array<point_t, 4> m_offsets = {{
                 {+0, -1},
        {-1, +0},          {+1, +0},
                 {+0, +1}
      }};
    // clang-format on
  };

  static constexpr inline c4_t c4 = {};
} // namespace ano

#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/experimental/point.hpp>

#include <array>
#include <range/v3/view/span.hpp>

namespace mln::experimental
{
  struct c8_t : neighborhood_facade<c8_t>
  {
  private:
    using point_t = mln::experimental::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 8> offsets() { return {m_offsets.data(), 8}; }
    static constexpr ::ranges::span<const point_t, 4> before_offsets() { return {m_offsets.data(), 4}; }
    static constexpr ::ranges::span<const point_t, 4> after_offsets() { return {m_offsets.data() + 4, 4}; }

    static constexpr int radial_extent() { return 1; }

    /// \brief Return the input ROI for 2D box.
    mln::experimental::box2d compute_input_region(mln::experimental::box2d roi) const
    {
      roi.inflate(1);
      return roi;
    }

    /// \brief Return the output ROI for 2D box.
    mln::experimental::box2d compute_output_region(mln::experimental::box2d roi) const
    {
      roi.inflate(-1);
      return roi;
    }


  private:
    // clang-format off
    static inline constexpr std::array<point_t, 8> m_offsets = {{
        {-1, -1}, {+0, -1}, {+1, -1},
        {-1, +0},           {+1, +0},
        {-1, +1}, {+0, +1}, {+1, +1}
      }};
    // clang-format on
  };

  static constexpr inline c8_t c8 = {};
} // namespace mln

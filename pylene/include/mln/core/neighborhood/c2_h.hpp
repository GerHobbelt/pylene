#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/experimental/point.hpp>

#include <array>
#include <range/v3/span.hpp>

namespace mln::experimental
{
  struct c2_h_t : neighborhood_facade<c2_h_t>
  {
  private:
    using point_t = mln::experimental::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 2> offsets() { return {m_offsets.data(), 2}; }
    static constexpr ::ranges::span<const point_t, 1> before_offsets() { return {m_offsets.data(), 1}; }
    static constexpr ::ranges::span<const point_t, 1> after_offsets() { return {m_offsets.data() + 1, 1}; }

    static constexpr int radial_extent() { return 1; }

    /// \brief Return the input ROI for 2D box.
    mln::experimental::box2d compute_input_region(mln::experimental::box2d roi) const
    {
      --roi.tl().x();
      ++roi.br().x();
      return roi;
    }

    /// \brief Return the output ROI for 2D box.
    mln::experimental::box2d compute_output_region(mln::experimental::box2d roi) const
    {
      // Fixme: check emptiness
      ++roi.tl().x();
      --roi.br().x();
      return roi;
    }


  private:
    // clang-format off
    static inline constexpr std::array<point_t, 2> m_offsets = {{
        {-1, +0},          {+1, +0},
      }};
    // clang-format on
  };

  static constexpr inline c2_h_t c2_h = {};
} // namespace mln::experimental

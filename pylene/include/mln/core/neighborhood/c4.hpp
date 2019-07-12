#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/experimental/point.hpp>

#include <array>
#include <range/v3/span.hpp>

namespace mln::experimental
{
  struct c4_t : neighborhood_facade<c4_t>
  {
  private:
    using point_t = mln::experimental::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 4> offsets() { return {m_offsets.data(), 4}; }
    static constexpr ::ranges::span<const point_t, 2> before_offsets() { return {m_offsets.data(), 2}; }
    static constexpr ::ranges::span<const point_t, 2> after_offsets() { return {m_offsets.data() + 2, 2}; }

    static constexpr int radial_extent() { return 1; }

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
} // namespace mln::experimental

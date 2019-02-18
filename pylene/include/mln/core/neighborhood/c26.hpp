#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>

#include <array>
#include <range/v3/span.hpp>

namespace mln::experimental
{
  struct c26_t : neighborhood_facade<c26_t>
  {
  private:
    using point_t = point<std::ptrdiff_t, 3>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 26> offsets() { return {m_offsets.data(), 26}; }
    static constexpr ::ranges::span<const point_t, 13> before_offsets() { return {m_offsets.data(), 13}; }
    static constexpr ::ranges::span<const point_t, 13> after_offsets() { return {m_offsets.data() + 13, 13}; }

  private:
    static inline constexpr std::array<point_t, 26> m_offsets = {{
        // clang-format off
        {-1, -1, -1}, {-1, -1, +0}, {-1, -1, +1},
        {-1, +0, -1}, {-1, +0, +0}, {-1, +0, +1},
        {-1, +1, -1}, {-1, +1, +0}, {-1, +1, +1},
        // ----
        {+0, -1, -1}, {+0, -1, +0}, {+0, -1, +1},
        {+0, +0, -1},               {+0, +0, +1},
        {+0, +1, -1}, {+0, +1, +0}, {+0, +1, +1},
        // ----
        {+1, -1, -1}, {+1, -1, +0}, {+1, -1, +1},
        {+1, +0, -1}, {+1, +0, +0}, {+1, +0, +1},
        {+1, +1, -1}, {+1, +1, +0}, {+1, +1, +1},
        // clang-format on
      }};
  };

  static constexpr inline c26_t c26 = {};
} // namespace mln::experimental

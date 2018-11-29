#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>

#include <array>
#include <range/v3/span.hpp>

namespace mln
{
  struct c4_new_t : neighborhood_facade<c4_new_t>
  {
  private:
    using point_t = point<std::ptrdiff_t, 2>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 4> offsets() { return {m_offsets.data(), 4}; }
    static constexpr ::ranges::span<const point_t, 2> before_offsets() { return {m_offsets.data(), 2}; }
    static constexpr ::ranges::span<const point_t, 2> after_offsets() { return {m_offsets.data() + 2, 2}; }

  private:
    // clang-format off
    static inline constexpr std::array<point_t, 4> m_offsets = {{
                 {-1, 0},
        {0, -1},          {0, 1},
                 {1, 0}
      }};
    // clang-format on
  };

  static constexpr inline c4_new_t c4_new = {};
} // namespace mln

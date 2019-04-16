#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>

#include <array>
#include <range/v3/span.hpp>

namespace mln::experimental
{
  struct c6_t : neighborhood_facade<c6_t>
  {
  private:
    using point_t = point<std::ptrdiff_t, 3>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    static constexpr ::ranges::span<const point_t, 6> offsets() { return {m_offsets.data(), 6}; }
    static constexpr ::ranges::span<const point_t, 3> before_offsets() { return {m_offsets.data(), 3}; }
    static constexpr ::ranges::span<const point_t, 3> after_offsets() { return {m_offsets.data() + 3, 3}; }

    static constexpr int radial_extent() { return 1; }

  private:
    static inline constexpr std::array<point_t, 6> m_offsets = {
        {{-1, +0, +0}, {+0, -1, +0}, {+0, +0, -1}, {+0, +0, +1}, {+0, +1, +0}, {+1, +0, +0}}};
  };

  static constexpr inline c6_t c6 = {};
} // namespace mln::experimental

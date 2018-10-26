#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>
#include <array>
#include <range/v3/span.hpp>

namespace mln
{
  struct c8_new_t : neighborhood_facade<c8_new_t>
  {
    using category        = constant_neighborhood_tag;
    using is_incremental  = std::false_type;
    using is_decomposable = std::false_type;

    constexpr ::ranges::span<const point2d, 8> offsets() const { return ::ranges::span<const point2d, 8>(m_offsets.data(), 8); }

  private:
    static inline constexpr std::array<point2d, 8> m_offsets = {{
        {-1, -1}, {-1, +0}, {-1, +1}, // _
        {+0, -1}, {+0, +1},           // _
        {+1, -1}, {+1, +0}, {+1, +1}  // _
      }};
  };


  static constexpr inline c8_new_t c8_new = {};
}

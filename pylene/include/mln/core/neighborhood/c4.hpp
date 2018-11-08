#pragma once

#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>
#include <array>
#include <range/v3/span.hpp>

namespace mln
{
  struct c4_new_t : neighborhood_facade<c4_new_t>
  {

    using category        = constant_neighborhood_tag;
    using is_incremental  = std::false_type;
    using is_decomposable = std::false_type;

    static constexpr ::ranges::span<const point2d, 4> offsets() { return {m_offsets.data(), 4}; }
    static constexpr ::ranges::span<const point2d, 2> before_offsets() { return {m_offsets.data(), 2}; }
    static constexpr ::ranges::span<const point2d, 2> after_offsets() { return {m_offsets.data() + 2, 2}; }

  private:
    static inline constexpr std::array<mln::point2d, 4> m_offsets = {
      {{-1,0}, {0,-1}, {0,1}, {1, 0}}
    };
  };


  static constexpr inline c4_new_t c4_new = {};
}

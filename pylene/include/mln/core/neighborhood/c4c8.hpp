#pragma once

#include <mln/core/box.hpp>
#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>

#include <range/v3/view/span.hpp>
#include <range/v3/view/stride.hpp>

namespace mln
{
  // Forward
  struct c4_t;
  struct c8_t;

  enum class c2d_type : std::ptrdiff_t
  {
    C8 = 1,
    C4 = 2
  };

  struct c4c8_t : neighborhood_facade<c4c8_t>
  {
  private:
    using point_t = mln::ndpoint<2, std::ptrdiff_t>;

  public:
    using category     = constant_neighborhood_tag;
    using incremental  = std::false_type;
    using decomposable = std::false_type;
    using separable    = std::false_type;

    c4c8_t(c2d_type t)
      : m_type(t)
      , m_first((static_cast<std::ptrdiff_t>(t) + 1) % 2)
    {
    }

    c4c8_t(const mln::c4_t&)
      : m_type(c2d_type::C4)
      , m_first(1)
    {
    }

    c4c8_t(const mln::c8_t&)
      : m_type(c2d_type::C8)
      , m_first(0)
    {
    }

    auto offsets() const
    {
      return (::ranges::span<const point_t>{m_offsets.data() + m_first, 8 - m_first} |
              ::ranges::views::stride(static_cast<std::ptrdiff_t>(m_type)));
    }
    auto before_offsets() const
    {
      return (::ranges::span<const point_t>{m_offsets.data() + m_first, 4 - m_first} |
              ::ranges::views::stride(static_cast<std::ptrdiff_t>(m_type)));
    }
    auto after_offsets() const
    {
      return (::ranges::span<const point_t>{m_offsets.data() + 4 + m_first, 4 - m_first} |
              ::ranges::views::stride(static_cast<std::ptrdiff_t>(m_type)));
    }

    static constexpr int radial_extent() { return 1; }

    /// \brief Return the input ROI for 2D box.
    mln::box2d compute_input_region(mln::box2d roi) const
    {
      roi.inflate(1);
      return roi;
    }

    /// \brief Return the output ROI for 2D box.
    mln::box2d compute_output_region(mln::box2d roi) const
    {
      roi.inflate(-1);
      return roi;
    }

    auto alternative() const { return c4c8_t(m_type == c2d_type::C4 ? c2d_type::C8 : c2d_type::C4); }

  private:
    const c2d_type       m_type;
    const std::ptrdiff_t m_first;
    // clang-format off
    static inline constexpr std::array<point_t, 8> m_offsets = {{
        {-1, -1}, {+0, -1}, {+1, -1}, {-1, +0}, {-1, +1}, {+1, +0}, {+1, +1}, {+0, +1}
      }};
    // clang-format on
  };
} // namespace mln
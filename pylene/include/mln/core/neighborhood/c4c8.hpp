#pragma once

#include <mln/core/box.hpp>
#include <mln/core/neighborhood/private/neighborhood_facade.hpp>
#include <mln/core/point.hpp>

#include <range/v3/view/concat.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_last.hpp>

namespace mln
{
  // Forward
  struct c4_t;
  struct c8_t;

  enum class c2d_type : std::ptrdiff_t
  {
    C4 = 4,
    C8 = 8
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

    constexpr c4c8_t(c2d_type t)
      : m_type(t)
    {
    }

    constexpr c4c8_t(const mln::c4_t&)
      : m_type(c2d_type::C4)
    {
    }

    constexpr c4c8_t(const mln::c8_t&)
      : m_type(c2d_type::C8)
    {
    }

    constexpr auto offsets() const
    {
      return ::ranges::span<const point_t>{m_offsets.data(), static_cast<std::ptrdiff_t>(m_type)};
    }

    auto before_offsets() const
    {
      return ::ranges::views::concat(m_offsets | ::ranges::views::take(2),
                                     m_offsets | ::ranges::views::take_last((static_cast<std::ptrdiff_t>(m_type) - 4)) |
                                         ::ranges::views::take(2));
    }

    auto after_offsets() const
    {
      return ::ranges::views::concat(m_offsets | ::ranges::views::take(4) | ::ranges::views::take_last(2),
                                     m_offsets |
                                         ::ranges::views::take_last((static_cast<std::ptrdiff_t>(m_type) - 4) / 2));
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

    constexpr auto alternative() const { return c4c8_t(m_type == c2d_type::C4 ? c2d_type::C8 : c2d_type::C4); }

  private:
    const c2d_type m_type;
    // clang-format off
    static inline constexpr std::array<point_t, 8> m_offsets = {{
        {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}
    }};
    // clang-format on
  };
} // namespace mln
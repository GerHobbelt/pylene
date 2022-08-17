#pragma once

#include <ano/core/concepts/pixel.hpp>

namespace ano
{
  /// \brief Pixel adaptor for weighted pixel range
  template <class Px, class W = void>
  struct weighted_pixel : ano::details::Pixel<weighted_pixel<Px, W>>
  {
    using point_type = typename Px::point_type;
    using reference  = typename Px::reference;
    using value_type = typename Px::value_type;

    decltype(auto) point() const noexcept { return m_pixel.point(); }
    decltype(auto) val() const noexcept { return m_pixel.val(); }
    W              weight() const noexcept { return m_weight; }
    void           shift(point_type dp) noexcept { m_pixel.shift(std::move(dp)); }

    weighted_pixel() = default;
    weighted_pixel(Px px, W w)
      : m_pixel{std::move(px)}
      , m_weight{w}
    {
    }

    Px m_pixel;
    W  m_weight;
  };


  template <class Px>
  struct weighted_pixel<Px, void> : ano::details::Pixel<weighted_pixel<Px, void>>
  {
    using point_type = typename Px::point_type;
    using reference  = typename Px::reference;
    using value_type = typename Px::value_type;

    decltype(auto) point() const noexcept { return m_pixel.point(); }
    decltype(auto) val() const noexcept { return m_pixel.val(); }
    constexpr int  weight() const noexcept { return 1; }
    void           shift(point_type dp) noexcept { m_pixel.shift(std::move(dp)); }

    weighted_pixel() = default;
    weighted_pixel(Px px)
      : m_pixel{std::move(px)}
    {
    }

  private:
    Px m_pixel;
  };

} // namespace ano
